#include "revert.h"
#include "../utils/getroot.h"
#include "../track/track.h"
#include "../commit/commit.h"
#include "../merge/merge.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void revert(std::string commitHash)
{
    std::string root = getroot();
    if (root.empty())
    {
        std::cout << "not in a mygit repository" << std::endl;
        return;
    }

    if (fs::exists(root + "/.mygit/MERGE_HEAD"))
    {
        std::cout
            << "cannot revert, a merge is still in progress "
            << "(finish or abort it first)"
            << std::endl;
        return;
    }

    // Never revert on top of uncommitted work.
    auto status = trackall();
    if (!status.untracked.empty() ||
            !status.modified.empty() ||
            !status.staged.empty() ||
            !status.deleted.empty())
    {
        std::cout
            << "cannot revert, you have uncommitted changes"
            << std::endl;
        return;
    }

    fs::path commitDir =
        fs::path(root) / ".mygit" / "objects" / commitHash;

    if (!fs::exists(commitDir) || !fs::is_directory(commitDir))
    {
        std::cout
            << "unknown commit: "
            << commitHash
            << std::endl;
        return;
    }

    // load_commit() walks .mygit/objects/<commitHash>/ and hands back the
    // files that were actually snapshotted there (it already skips
    // "index" and "metadata"), so this is the same object-store read the
    // rest of the codebase uses for a commit's contents.
    auto snapshot = load_commit(root, commitHash);

    if (snapshot.empty())
    {
        std::cout
            << "commit "
            << commitHash
            << " has no recorded files"
            << std::endl;
        return;
    }

    // Revert restores the tree to exactly match that commit, so anything
    // currently tracked that isn't part of the target snapshot has to go.
    auto currentIndex = load_index(root);
    for (auto& [file, hash] : currentIndex)
    {
        if (snapshot.find(file) == snapshot.end())
        {
            std::error_code ec;
            fs::remove(fs::path(root) / file, ec);
        }
    }

    // Copy every file straight out of the commit's object directory into
    // the working tree.
    for (auto& [file, hash] : snapshot)
    {
        fs::path source = commitDir / file;
        fs::path destination = fs::path(root) / file;

        fs::create_directories(destination.parent_path());

        fs::copy_file(
                source,
                destination,
                fs::copy_options::overwrite_existing);
    }

    rebuild_index_from_worktree();

    commit("Revert to commit " + commitHash);

    std::cout
        << "Reverted working tree to commit "
        << commitHash
        << std::endl;
}
