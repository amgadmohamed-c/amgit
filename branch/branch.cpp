#include "branch.h"
#include "../utils/getroot.h"
#include "../track/track.h"
#include "../log/log.h"
#include <filesystem>
#include <fstream>
#include <iostream>

void branch(){
    std::string path = getroot();
    std::string head = loadhead(path);

    std::cout << "your current branch is: " << head.substr(head.find("/") + 7) << std::endl;
}

void createbranch(std::string branchname){
    std::string path = getroot();
    std::string head = loadhead(path);
    std::string parent = loadbranch(path,head);


    ofstream file(path + "/.mygit/refs/heads/" + branchname);


    if (file.is_open()){
        file << parent;
        file.close();}
    std::cout << "branch created successfully" << std::endl;
}
void checkout(std::string branchname){
    auto result = trackall();
    if(result.untracked.size() !=0 ||result.modified.size() !=0||result.staged.size() !=0){
        std::cout << "cant checkout, you have pending changes" << std::endl;
        return;
    }
    std::string path = getroot();
    std::string head = loadhead(path);
    if(branchname == head.substr(head.find("/") + 7)){
        std::cout << "you are already on this branch" << std::endl;
        return;
    }
    bool clean =  false;
    std::filesystem::directory_iterator myheaddir(path + "/.mygit/refs/heads");
    for(auto & entry : myheaddir){
        if(entry.path().filename() == branchname){
            clean = true;
            break;}
        continue;
    }
    if(clean){
        std::string target = loadbranch(path,"refs/heads/"+branchname);
        std::filesystem::directory_iterator workingtree(path ) ;
        for(auto & entry : workingtree){
            if(entry.path().filename() == ".mygit"){
                continue;
            }
            std::filesystem::remove_all(entry.path());
        }
        std::filesystem::recursive_directory_iterator mycommitdir(path + "/.mygit/objects/"+target);
        for(auto & entry : mycommitdir){
            if(!entry.is_regular_file()){
                continue;
            }
            if(entry.path().filename() == "metadata" || entry.path().filename() == "index"){
                continue;
            }

            auto relative = std::filesystem::relative(entry.path(),path + "/.mygit/objects/"+target);
            auto destination = std::filesystem::path(path)/relative;
            std::filesystem::create_directories(destination.parent_path());
            std::filesystem::copy_file(entry.path(),destination ,std::filesystem::copy_options::overwrite_existing);
        }
        std::filesystem::copy_file(
                path + "/.mygit/objects/" + target + "/index",
                path + "/.mygit/index",
                std::filesystem::copy_options::overwrite_existing
                );
        std::ofstream file(path + "/.mygit/HEAD");
        if (file.is_open()){
            file << "refs/heads/"+ branchname;
            file.close();}

        std::cout << "checkout successful" << std::endl ;
        return;
    }
    if(!clean){
        std::cout << "branch doesnt exist" << std::endl;
        return;
    }







}
