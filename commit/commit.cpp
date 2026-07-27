#include "commit.h"
#include <filesystem>
#include <iostream>
#include <string>
#include "../track/track.h"
#include "../utils/getroot.h"
#include "../utils/hash_file.h"
#include "../utils/sha1.hpp"
#include <chrono>
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
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&time_now);
   SHA1 sha1;
   std::string commithash = std::to_string(local_tm.tm_year)+std::to_string(local_tm.tm_mon)+std::to_string(local_tm.tm_mday)+std::to_string(local_tm.tm_hour)+std::to_string(local_tm.tm_min)+std::to_string(local_tm.tm_sec)+message + ParentCommit ;
   sha1.update(commithash)  ;
   commithash = sha1.final();
    std::filesystem::create_directory(path+"/.mygit/objects/"+commithash);

    for(auto& file : index){
        std::filesystem::path source = std::filesystem::path(path)/file.first ;
        std::filesystem::path destination = std::filesystem::path(path)/".mygit/objects"/commithash/file.first;
        filesystem::create_directories(destination.parent_path());
        std::filesystem::copy_file(source,destination , std::filesystem::copy_options::overwrite_existing);



    }


    std::filesystem::copy_file(path+"/.mygit/index",path+"/.mygit/objects/"+commithash+"/index" , std::filesystem::copy_options::overwrite_existing);
    std::ofstream metadata(path+"/.mygit" +"/objects/" + commithash + "/metadata");
    if(metadata.is_open()){
        metadata << "commit :" << commithash << std::endl;
        metadata << "parent :" << ParentCommit << std::endl;
        metadata << "message :" << message << std::endl  ;
        metadata << "time :" << std::to_string(time(nullptr)) << std::endl;
        metadata.close();
    }

    std::ofstream ref(path+"/.mygit/"+curret_head);
    if(ref.is_open()){
        ref << commithash << std::endl;
        ref.close();
    }
    else{
        std::cout << "could not create ref nor open file" << std::endl;

    }
    std::cout << "Committed as :  " << message << std::endl;
}

