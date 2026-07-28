#include "./merge.h"
#include "../utils/getcommonanc.h"
#include "../utils/getroot.h"
#include "../track/track.h"
#include "../branch/branch.h"
#include <set>
void merge(std::string upcomingbranch){
    auto result = trackall();
    if(result.untracked.size() !=0 ||result.modified.size() !=0||result.staged.size() !=0){
        std::cout << "cant merge, you have pending changes" << std::endl;
        return;
    }
    std::string root = getroot();
    std::string anc = getcommonanc(upcomingbranch) ;
    std::string head = loadhead(root) ;
    std::string branch =loadbranch(root, head) ;
    std::string comingbranch = loadbranch(root, "refs/heads/"+upcomingbranch) ;
    if(anc.empty()){
        return ;
    }
    if(anc == branch){
        std::ofstream file;
        checkout(upcomingbranch);
        file.open(root+"/.mygit/"+head) ;
        file << comingbranch;
        file.close();
        file.open(root+"/.mygit/HEAD") ;
        file << head ;
        file.close();
        std::cout << "fast forward merge" << std::endl;
        return ;
    }
    if(anc == comingbranch){
        std::cout << "already up to date" << std::endl;
        return ;
    }
    auto  anccommit= load_commit(root,anc);
    auto  branchcommit= load_commit(root,branch);
    auto  comingbranchcommit= load_commit(root,comingbranch);
    std::unordered_map<std::string , MergeResult> mergelist;
    std::ofstream merge(".mygit/MERGE_HEAD") ;
    merge << branch << std::endl;
    merge <<comingbranch << std::endl;
    merge.close();
    std::ofstream mergeconflicts(".mygit/MERGE_CONFLICTS") ;

    std::set<std::string> files;

    for(auto &[f,_] : anccommit)
        files.insert(f);

    for(auto &[f,_] : branchcommit)
        files.insert(f);

    for(auto &[f,_] : comingbranchcommit)
        files.insert(f);
    bool conflict = false;
    std::vector<std::string> conflictlist;
    for (auto& file : files)
    {
        std::string ancfiles = anccommit[file];
        std::string branchfiles = branchcommit[file];
        std::string comingfiles = comingbranchcommit[file];

        if (branchfiles == comingfiles)
        {
            // identical
            mergelist[file] = TAKECURRENT;
        }
        else if (ancfiles == branchfiles)
        {
            // only incoming changed
            mergelist[file] = TAKEINCOMING;
        }
        else if (ancfiles == comingfiles)
        {
            // only current changed
            mergelist[file] = TAKECURRENT;
        }
        else
        {
            std::cout
                << "Conflict in "
                << file
                << '\n';
            conflictlist.push_back(file);

            conflict = true;
        }
    }
    if(conflict){
        for(auto & file : conflictlist){
            mergeconflicts << file << std::endl;
        }
        std::cout << "merge conflict" << std::endl;
        std::cout << "conflict list" << std::endl;
        for(auto & file : conflictlist){
            std::cout <<"   " <<file << std::endl;
        }

        return ;
    }
    for(auto & [file,result] : mergelist){
        if(result == TAKECURRENT){
            continue;
        }

        else if(result == TAKEINCOMING){
            string source= root+"/.mygit/objects/"+comingbranch+"/"+file;
            std::filesystem::path destination = std::filesystem::path(root) / file;

            std::filesystem::create_directories(destination.parent_path());
            std::filesystem::copy_file(source,destination , std::filesystem::copy_options::overwrite_existing);
        }
    }

}
