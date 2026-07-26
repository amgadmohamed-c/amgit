#include "commit.h"
#include <filesystem>
#include <iostream>
#include <string>
#include "../track/track.h"
#include "../utils/getroot.h"
void commit(std::string message ){
    std::string path = getroot();
    if(path.empty()){
        std::cout << "not in a mygit repository" << std::endl;
        return;
    }
    auto index = load_index(path);
    if(index.empty()){
        std::cout << "No files to commit" << std::endl;
        return;
    }
    std::string curret_head = loadhead(path);
    std::string ParentCommit ;
    std::ifstream writeref(path+"/.mygit/"+curret_head);
    if(writeref.is_open()){
        std::getline(writeref,ParentCommit);
        writeref.close();

    }
    if(!ParentCommit.empty()){
   auto  parentindex= load_parent_index(path+"/.mygit/objects/"+ParentCommit+"/index");
   if(parentindex == index){
       std::cout << "nothing to commit" << std::endl;
       return;
   }
    }
    std::ifstream file(path+"/.mygit/commit_count");
    std::string commitcount;
    if(file.is_open()){
        std::getline(file,commitcount);
        file.close();
    }
    int newcommitcount = 0 ;
    if(!commitcount.empty()){
        newcommitcount = std::stoi(commitcount);
    }
    commitcount = std::to_string(newcommitcount+1);
    std::filesystem::create_directory(path+"/.mygit/objects/"+commitcount);

    for(auto& file : index){
        std::filesystem::path source = std::filesystem::path(path)/file.first ;
        std::filesystem::path destination = std::filesystem::path(path)/".mygit/objects"/commitcount/file.first;
        filesystem::create_directories(destination.parent_path());
        std::filesystem::copy_file(source,destination , std::filesystem::copy_options::overwrite_existing);



    }

    std::ofstream commitcountfile(path+"/.mygit/commit_count");
    if(commitcountfile.is_open()){
        commitcountfile << commitcount << std::endl;
        commitcountfile.close();
    }

    std::filesystem::copy_file(path+"/.mygit/index",path+"/.mygit/objects/"+commitcount+"/index" , std::filesystem::copy_options::overwrite_existing);
    std::ofstream metadata(path+"/.mygit" +"/objects/" + commitcount + "/metadata");
    if(metadata.is_open()){
        metadata << "commit :" << commitcount << std::endl;
        metadata << "parent :" << ParentCommit << std::endl;
        metadata << "message :" << message << std::endl  ;
        metadata << "time :" << std::to_string(time(nullptr)) << std::endl;
        metadata.close();
    }

    std::ofstream ref(path+"/.mygit/"+curret_head);
    if(ref.is_open()){
        ref << commitcount << std::endl;
        ref.close();
    }
    else{
        std::cout << "could not create ref nor open file" << std::endl;

    }
    std::cout << "Committed as :  " << message << std::endl;
}

