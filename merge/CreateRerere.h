// rerere/rerere.h
#pragma once
#include <string>
#include <optional>

struct ConflictHunk {
    std::string oursLabel, theirsLabel;
    std::string oursContent, theirsContent;
    size_t startLine, endLine; // location in the working file
};

// Call once at repo root before using any rerere_* function.
// (Also called lazily/idempotently by the functions below, so you
// don't strictly have to remember to call it yourself.)
void rerere_init(const std::string& repoRoot);

// Returns the cached resolution text if one exists for this hunk, else nullopt.
std::optional<std::string> rerere_try_resolve(const ConflictHunk& hunk);

// Call after the user resolves a conflicted hunk (e.g. during `amgit add`).
void rerere_record(const ConflictHunk& hunk, const std::string& resolvedText);

// Internal helpers, exposed for testing.
std::string rerere_normalize(const std::string& text);
std::string rerere_hash_hunk(const ConflictHunk& hunk);

// ---- Bridge API used directly by merge.cpp / commit.cpp ----

// Called from merge_file() when both sides differ from the ancestor.
// If rerere has seen this exact conflict before, writes the cached
// resolution straight into `output` and returns true.
bool try_rerere(const std::string& curr, const std::string& inc, const std::string& output);

// Called from merge_file() right before it writes conflict markers.
// Stashes the preimage AND remembers (file -> hash) in MERGE_RR so that
// learn_rerere() can find the resolution later, once the user has
// actually fixed the conflict markers in `output`.
void save_preimage(const std::string& curr, const std::string& inc, const std::string& output);

// Called from commit() when a MERGE_HEAD is present. Walks MERGE_RR,
// and for every file that no longer contains conflict markers, records
// the current (resolved) file content as the postimage for that hash.
void learn_rerere();
