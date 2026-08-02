#pragma once
#include <string>

// Restores the working tree to exactly match the snapshot stored for
// `commitHash` in .mygit/objects/<commitHash>/, then records that as a
// brand new commit on top of the current HEAD (history isn't rewritten).
//
// Refuses to run if there are any uncommitted changes (untracked,
// modified, staged, or deleted files), or if a merge is in progress.
void revert(std::string commitHash);
