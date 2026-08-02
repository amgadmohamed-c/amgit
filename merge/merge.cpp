#include "./merge.h"
#include "../utils/getcommonanc.h"
#include "../utils/getroot.h"
#include "../track/track.h"
#include "../branch/branch.h"
#include <set>
#include <algorithm>
#include <fstream>
#include "../branch/branch.h"
#include "../commit/commit.h"

#include <set>
#include <fstream>
#include <filesystem>
#include "../utils/hash_file.h"

#include "CreateRerere.h"

void merge(std::string upcomingbranch)
{
    auto status = trackall();
    if (!status.untracked.empty() ||
            !status.modified.empty() ||
            !status.staged.empty())
    {
        std::cout
            << "cannot merge, you have pending changes"
            << std::endl;

        return;
    }

    std::string root = getroot();

    rerere_init(root);

    std::string ancestor =
        getcommonanc(upcomingbranch);

    std::string head =
        loadhead(root);

    std::string currentCommit =
        loadbranch(root, head);

    std::string incomingCommit =
        loadbranch(
                root,
                "refs/heads/" + upcomingbranch);

    if (ancestor.empty())
    {
        std::cout
            << "could not determine common ancestor"
            << std::endl;

        return;
    }


    if (ancestor == currentCommit)
    {
        checkout(upcomingbranch);

        std::ofstream file(
                root + "/.mygit/" + head);

        file << incomingCommit;
        file.close();

        std::ofstream headfile(
                root + "/.mygit/HEAD");

        headfile << head;
        headfile.close();

        std::cout
            << "Fast-forward merge."
            << std::endl;

        return;
    }


    if (ancestor == incomingCommit)
    {
        std::cout
            << "Already up to date."
            << std::endl;

        return;
    }


    auto ancestorFiles =
        load_commit(root, ancestor);

    auto currentFiles =
        load_commit(root, currentCommit);

    auto incomingFiles =
        load_commit(root, incomingCommit);

    std::unordered_map<
        std::string,
        MergeResult> mergeList;

    std::ofstream mergeHead(
            root + "/.mygit/MERGE_HEAD");

    mergeHead << currentCommit << '\n';
    mergeHead << incomingCommit << '\n';

    mergeHead.close();

    std::ofstream mergeConflicts(
            root + "/.mygit/MERGE_CONFLICTS");

    std::set<std::string> files;

    for (auto& [f, _] : ancestorFiles)
        files.insert(f);

    for (auto& [f, _] : currentFiles)
        files.insert(f);

    for (auto& [f, _] : incomingFiles)
        files.insert(f);

    bool conflict = false;

    std::vector<std::string> conflictList;

    for (auto& file : files)
    {
        std::string anc =
            ancestorFiles[file];

        std::string curr =
            currentFiles[file];

        std::string inc =
            incomingFiles[file];


        if (curr == inc)
        {
            mergeList[file] =
                TAKECURRENT;
        }


        else if (anc == curr)
        {
            mergeList[file] =
                TAKEINCOMING;

        }


        else if (anc == inc)
        {
            mergeList[file] =
                TAKECURRENT;
        }


        else
        {
            auto result = merge_file(
                    root +
                    "/.mygit/objects/" +
                    ancestor +
                    "/" +
                    file,

                    root +
                    "/.mygit/objects/" +
                    currentCommit +
                    "/" +
                    file,

                    root +
                    "/.mygit/objects/" +
                    incomingCommit +
                    "/" +
                    file,

                    root +
                    "/" +
                    file);

            if (result == CONFLICT)
            {
                conflict = true;

                conflictList.push_back(
                        file);
            }
        }
    }


    if (conflict)
    {
        for (auto& file : conflictList)
        {
            mergeConflicts
                << file
                << '\n';
        }

        std::cout
            << "\nMerge conflict detected.\n"
            << std::endl;

        for (auto& file : conflictList)
        {
            std::cout
                << "   "
                << file
                << std::endl;
        }

        std::cout
            << "\nResolve the files and "
            << "commit the merge."
            << std::endl;

        return;
    }

    for (auto& [file, result] : mergeList)
    {
        if (result == TAKECURRENT)
        {
            continue;
        }

        std::string source =
            root +
            "/.mygit/objects/" +
            incomingCommit +
            "/" +
            file;

        std::filesystem::path destination =
            std::filesystem::path(root)
            / file;

        std::filesystem::create_directories(
                destination.parent_path());

        std::filesystem::copy_file(
                source,
                destination,
                std::filesystem::copy_options::
                overwrite_existing);
    }

for(auto& [f,r] : mergeList)
{
    std::cout
        << f
        << " "
        << r
        << std::endl;
}

rebuild_index_from_worktree();

auto index = load_index(root);
for (auto  entry: index){
    std::cout << entry.first << " " << entry.second << std::endl;

}

    commit(
            "Merge branch '" +
            upcomingbranch +
            "'");

    std::filesystem::remove(
            root + "/.mygit/MERGE_HEAD");

    std::filesystem::remove(
            root + "/.mygit/MERGE_CONFLICTS");

    std::cout
        << "Merge completed successfully."
        << std::endl;
}

std::vector<std::string> read_file(std::string path)
{
    std::vector<std::string> lines;

    std::ifstream file(path);

    std::string line;

    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    return lines;
}

MergeLineResult merge_file(
        const std::string& ancestor,
        const std::string& current,
        const std::string& incoming,
        const std::string& output)
{
    auto a = read_file(ancestor);
    auto c = read_file(current);
    auto i = read_file(incoming);

    std::ofstream out(output);

    size_t max_lines =
        std::max({a.size(), c.size(), i.size()});

    for (size_t x = 0; x < max_lines; x++)
    {
        std::string anc =
            x < a.size() ? a[x] : "";

        std::string curr =
            x < c.size() ? c[x] : "";

        std::string inc =
            x < i.size() ? i[x] : "";

        if (curr == inc)
        {
            out << curr << '\n';
        }

        else if (anc == curr)
        {
            out << inc << '\n';
        }

        else if (anc == inc)
        {
            out << curr << '\n';
        }

        else

        {
    if(try_rerere(curr, inc, output))
    {
        out.close();
        return AUTO_MERGED;
    }

    save_preimage(curr, inc, output);

    out << "<<<<<<< CURRENT\n";
    out << curr << '\n';
    out << "=======\n";
    out << inc << '\n';
    out << ">>>>>>> INCOMING\n";

    out.close();

    return CONFLICT;
}
    }

    out.close();

    return AUTO_MERGED;

}


void save_index(
    const std::string& root,
    const std::unordered_map<
        std::string,
        std::string>& index)
{
    std::ofstream file(
            root + "/.mygit/index");

    for (const auto& [path, hash] : index)
    {
        file
            << path
            << ' '
            << hash
            << '\n';
    }
}


void rebuild_index_from_worktree()
{
    std::string root = getroot();

    std::unordered_map<
        std::string,
        std::string> index;

    for (auto const& entry :
         std::filesystem::
         recursive_directory_iterator(root))
    {
        if (!entry.is_regular_file())
            continue;

        auto relative =
            std::filesystem::relative(
                    entry.path(),
                    root);

        std::string file =
            relative.string();

        if (file.rfind(".mygit",0) == 0)
            continue;

        index[file] =
            hash_file(
                    entry.path().string());
    }

    save_index(root,index);
}
