#include "log.h"
#include "../track/track.h"
#include <filesystem>
#include "../utils/getroot.h"
void log(){
   std::filesystem::path path =  getroot();
   auto head = loadhead(path.string());
   std::cout << "head is " << head << std::endl;
   auto branch = loadbranch(path.string() ,head);
   while(!branch.empty()){
     print_metadata(branch);

     branch = get_parent(branch);
   }
}
void print_metadata(std::string commit){
    std::filesystem::path path = std::filesystem::current_path();
    std::filesystem::path metadata = std::filesystem::path(path)/".mygit/objects"/commit/"metadata";
    std::ifstream file(metadata.string());
    if(file.is_open()){
        std::string line;
        while(std::getline(file,line)){
            std::cout << line << std::endl;
        }
        file.close();
    }
}
std::string get_parent(std::string commit){
    std::filesystem::path path = std::filesystem::current_path();
    std::filesystem::path metadata = std::filesystem::path(path)/".mygit/objects"/commit/"metadata";
    std::ifstream file(metadata.string());
    std::string parent;
    if(file.is_open()){
        std::string line;
        while(std::getline(file,line)){
            if(line.find("parent :")!=std::string::npos){
                parent = line.substr(line.find(":")+1)  ;
            }
        }
        file.close();
   }
 return parent;
}
