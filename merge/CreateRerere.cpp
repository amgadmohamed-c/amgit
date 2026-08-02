// rerere/rerere.cpp
#include "./CreateRerere.h"
#include "../utils/sha1.hpp"   // reuse your existing SHA-1 helper
#include "../utils/getroot.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

static std::string g_cacheDir;
static std::string g_repoRoot;

static std::string preimage_path(const std::string& hash) {
    return g_cacheDir + "/" + hash + "/preimage";
}

static std::string postimage_path(const std::string& hash) {
    return g_cacheDir + "/" + hash + "/postimage";
}

void rerere_init(const std::string& repoRoot) {
    g_repoRoot = repoRoot;
    // NOTE: this used to be repoRoot + "/.amgit/rr-cache", which doesn't
    // match the ".mygit" layout the rest of the codebase uses (see
    // commit.cpp / add.cpp / merge.cpp) — nothing was ever actually being
    // written where it could be found again. Fixed to live under .mygit.
    g_cacheDir = repoRoot + "/.mygit/rr-cache";
    fs::create_directories(g_cacheDir);
}

// rerere_init() was never actually being called anywhere in the codebase,
// so g_cacheDir was always empty and every path resolved relative to CWD
// instead of the repo. Everything below now lazily self-inits off getroot()
// so callers don't have to remember to wire it up.
static void ensure_init() {
    if (g_cacheDir.empty()) {
        std::string root = getroot();
        if (!root.empty()) {
            rerere_init(root);
        }
    }
}

std::string rerere_normalize(const std::string& text) {
    std::istringstream iss(text);
    std::ostringstream oss;
    std::string line;
    while (std::getline(iss, line)) {
        while (!line.empty() && (line.back() == ' ' || line.back() == '\t'))
            line.pop_back();
        oss << line << '\n';
    }
    return oss.str();
}

std::string rerere_hash_hunk(const ConflictHunk& hunk) {
    std::string normalized =
        rerere_normalize(hunk.oursContent) + '\x00' + rerere_normalize(hunk.theirsContent);
    return sha1_hex(normalized);
}

std::optional<std::string> rerere_try_resolve(const ConflictHunk& hunk) {
    ensure_init();

    std::string hash = rerere_hash_hunk(hunk);
    std::ifstream post(postimage_path(hash));
    if (!post) return std::nullopt;

    std::ostringstream oss;
    oss << post.rdbuf();
    return oss.str();
}

void rerere_record(const ConflictHunk& hunk, const std::string& resolvedText) {
    ensure_init();

    std::string hash = rerere_hash_hunk(hunk);
    std::string dir = g_cacheDir + "/" + hash;
    fs::create_directories(dir);

    std::ofstream pre(preimage_path(hash));
    pre << rerere_normalize(hunk.oursContent) << "---\n" << rerere_normalize(hunk.theirsContent);

    std::ofstream post(postimage_path(hash));
    post << resolvedText;
}

// ---------------- Bridge API used by merge.cpp / commit.cpp ----------------

bool try_rerere(const std::string& curr, const std::string& inc, const std::string& output) {
    ensure_init();

    ConflictHunk hunk;
    hunk.oursContent = curr;
    hunk.theirsContent = inc;

    auto resolved = rerere_try_resolve(hunk);
    if (!resolved) return false;

    std::ofstream out(output, std::ios::trunc);
    if (!out) return false;
    out << *resolved;
    return true;
}

void save_preimage(const std::string& curr, const std::string& inc, const std::string& output) {
    ensure_init();
    if (g_repoRoot.empty()) return; // not in a repo, nothing sane to do

    ConflictHunk hunk;
    hunk.oursContent = curr;
    hunk.theirsContent = inc;
    std::string hash = rerere_hash_hunk(hunk);

    // Stash the preimage now; the postimage will be filled in later by
    // learn_rerere() once the user has actually resolved the conflict.
    std::string dir = g_cacheDir + "/" + hash;
    fs::create_directories(dir);
    std::ofstream pre(preimage_path(hash));
    pre << rerere_normalize(hunk.oursContent) << "---\n" << rerere_normalize(hunk.theirsContent);

    // Remember which working-tree file this hash belongs to, so that at
    // commit time we know where to look for the resolved text.
    std::string relpath = fs::relative(output, g_repoRoot).string();

    std::ofstream rr(g_repoRoot + "/.mygit/MERGE_RR", std::ios::app);
    rr << relpath << '\t' << hash << '\n';
}

void learn_rerere() {
    ensure_init();
    if (g_repoRoot.empty()) return;

    std::string rrPath = g_repoRoot + "/.mygit/MERGE_RR";
    std::ifstream rr(rrPath);
    if (!rr) return; // no pending conflicts recorded this merge

    std::string relpath, hash;
    while (rr >> relpath >> hash) {
        std::ifstream resolvedFile(g_repoRoot + "/" + relpath);
        if (!resolvedFile) continue;

        std::ostringstream oss;
        oss << resolvedFile.rdbuf();
        std::string content = oss.str();

        // If markers are still there, the user never actually resolved
        // this one (or resolved it by hand-editing something else) —
        // don't learn a bogus resolution.
        if (content.find("<<<<<<<") != std::string::npos) {
            continue;
        }

        fs::create_directories(g_cacheDir + "/" + hash);
        std::ofstream post(postimage_path(hash));
        post << content;
    }

    rr.close();
    fs::remove(rrPath);
}
