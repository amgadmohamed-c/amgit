# System Architecture

## Overview

`mygit` is a simplified Git implementation written in C++ that reproduces the core workflow of a distributed version control system while intentionally simplifying the internal storage model.

Unlike Git, which stores **blob**, **tree**, and **commit** objects separately, **mygit stores complete file snapshots inside each commit object**. This significantly reduces implementation complexity while preserving the user-facing workflow:

```
Working Directory
        │
        ▼
   Status Detection
        │
        ▼
      add()
        │
        ▼
     Index File
        │
        ▼
    commit()
        │
        ▼
 Commit Snapshot
        │
        ▼
 Branch Reference
        │
        ▼
       HEAD
```

The repository follows the same conceptual pipeline as Git:

* Working Tree
* Staging Area
* Commit History
* Branch References
* HEAD Pointer

---

# Repository Layout

Running

```bash
mygit init
```

creates the following repository structure:

```text
.mygit
├── HEAD
├── index
├── commit_count
├── objects/
│   └── <commit hash>/
│       ├── metadata
│       ├── index
│       └── <tracked project files>
├── refs/
│   ├── heads/
│   │   └── main
│   └── tags/
├── logs/
└── info/
```

Each component has a single responsibility.

## HEAD

HEAD stores the currently checked-out branch.

Example:

```text
refs/heads/main
```

Instead of directly referencing a commit, HEAD references a branch, allowing branch movement without modifying HEAD itself.

---

## refs/heads

Each file inside `refs/heads` represents a branch.

Example:

```
main
feature-login
testing
```

The content of each file is simply the hash of its latest commit.

```
7ab92fd61...
```

Creating a branch therefore only requires creating another reference file pointing to the same commit.

---

## objects/

Every commit creates a directory inside `.mygit/objects`.

Example:

```
objects/
    8d7f53...
```

Unlike Git, which separates commits, blobs and trees, every commit directory contains:

* metadata
* commit index
* full snapshot of every tracked file

This allows checkout, revert and merge to reconstruct repository state without traversing multiple object types.

---

# Snapshot-Based Storage

One of the biggest architectural decisions in this project is using **full snapshots** instead of blobs.

When a commit is created:

```
Working Directory
       │
       ▼
Tracked Files
       │
       ▼
Copy every tracked file
       │
       ▼
objects/<commit hash>/
```

Advantages:

* Extremely simple checkout
* Easy revert implementation
* Straightforward merge logic
* No tree reconstruction

Trade-off:

* More storage consumption than Git
* Duplicate unchanged files across commits

This trade-off was intentionally made to prioritize readability and educational value.

---
# Performance Characteristics


Although mygit stores complete snapshots instead of Git's blob-based object model, it remains a fast version control system for the repositories it is designed to manage.

Most operations perform only a single traversal of the working directory and use hash maps for constant-time lookups, making commands such as status, add, log, and branch highly responsive even for projects containing hundreds or thousands of files.

The primary performance trade-off occurs during commit, checkout, and revert, where complete repository snapshots are copied rather than only changed objects. While this requires more disk I/O than Git, the implementation avoids the overhead of reconstructing object trees, resolving blob references, and traversing a complex object database.

Overall, the architecture favors simplicity, predictable linear-time performance, and maintainability over maximum storage efficiency. For the scale of repositories this project targets, mygit provides fast execution while keeping the implementation considerably simpler than Git's internal design.

---
# Repository State Model

Almost every command operates on three versions of the repository.

```
Working Directory
        │
        ▼
      Index
        │
        ▼
 Last Commit Snapshot
```

These three states are sufficient to implement:

* status
* add
* commit
* checkout
* merge
* revert

---

# Status Detection

The implementation of `status` is centered around **two comparisons**.

## Step 1

Compare

```
Working Directory
        vs
Index
```

This identifies

* modified files
* newly created files

---

## Step 2

Compare

```
Index
      vs
HEAD Commit
```

This identifies

* staged files
* committed files

---

Internally, `trackall()` builds three hash maps:

```
Working Directory
relative path
      │
      ▼
SHA-1 Hash

Index
relative path
      │
      ▼
Stored Hash

HEAD Commit Index
relative path
      │
      ▼
Stored Hash
```

Using these maps it classifies every file into:

* Untracked
* Modified
* Staged
* Deleted
* Clean

This design avoids scanning files multiple times and gives every command a consistent view of repository state.

---

# Staging Area

The staging area is implemented as a single index file.

```
.mygit/index
```

Each entry stores

```
relative/path hash
```

When `add` is executed:

1. `trackall()` discovers modified and untracked files.
2. The selected file's hash is inserted into the index.
3. The index file is rewritten.

Only files present inside the index become part of the next commit.

---

# Commit Creation

When `commit` executes:

```
Index
   │
   ▼
Read HEAD
   │
   ▼
Determine Parent Commit
   │
   ▼
Generate SHA-1 Commit ID
   │
   ▼
Create Commit Directory
   │
   ▼
Copy Snapshot
   │
   ▼
Write Metadata
   │
   ▼
Move Branch Pointer
```

A commit stores:

* commit hash
* parent commit
* second parent (merge commits)
* timestamp
* commit message
* repository snapshot
* commit index

Commit hashes are generated using SHA-1 over:

* timestamp
* commit message
* parent hash(es)

This guarantees deterministic commit identifiers while keeping the implementation lightweight.

---

# Branch Architecture

Branches are lightweight references.

```
refs/heads/main
            │
            ▼
        Commit A

refs/heads/dev
            │
            ▼
        Commit A
```

Creating a branch simply copies the current commit hash into another reference file.

No repository data is duplicated.

---

# Checkout

Checkout first guarantees repository safety.

It refuses to continue if there are

* staged files
* modified files
* untracked files

Once clean:

```
Target Branch
       │
       ▼
Latest Commit
       │
       ▼
Delete Working Tree
       │
       ▼
Restore Snapshot
       │
       ▼
Update HEAD
```

Because every commit stores a complete snapshot, checkout becomes a direct filesystem copy rather than reconstructing trees.

---

# Merge

Merge implements a traditional **three-way merge**.

It first computes:

* current commit
* incoming commit
* common ancestor

```
Ancestor
   │
   ├─────────────┐
   ▼             ▼
Current      Incoming
```

Every file from the union of these snapshots is compared to determine:

* unchanged
* modified on one branch
* modified on both branches
* conflicts

When conflicts occur, merge markers are written into the affected files.

The implementation also integrates **rerere (reuse recorded resolution)**.

Resolved conflicts are stored so identical future conflicts can be resolved automatically.

Fast-forward merges are detected and handled separately without creating an additional merge commit.

---

# Revert

Revert restores the repository to a previous commit **without deleting history**.

The workflow is:

```
Target Commit
       │
       ▼
Load Snapshot
       │
       ▼
Replace Working Tree
       │
       ▼
Update Index
       │
       ▼
Create Revert Commit
```

Unlike checkout, revert records the restoration as a brand-new commit, preserving repository history.

---

# Log Traversal

The log command starts from the current branch HEAD.

```
HEAD
 │
 ▼
Commit
 │
 ▼
Parent
 │
 ▼
Parent
 │
 ▼
...
```

For every commit it prints:

* abbreviated hash
* parent commit(s)
* timestamp
* commit message

Traversal continues until the root commit is reached.

---


# Design Decisions

This project intentionally differs from Git in several ways:

| Git                      | mygit                         |
| ------------------------ | ----------------------------- |
| Blob + Tree object model | Full snapshot storage         |
| Delta-friendly           | Simpler implementation        |
| Object graph traversal   | Direct filesystem restoration |
| Complex checkout         | Snapshot copy                 |
| Highly optimized         | Educational and maintainable  |

The goal was not to clone Git's internals exactly, but to build a clean, understandable implementation of the most important version-control concepts while keeping the codebase approachable.

