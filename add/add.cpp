#include "add.h"
#include "../track/track.h"
#include <filesystem>
void add(std::string path , std::string filename){
auto index = load_index(path);
auto status = trackall(path);
if(status.empty()){
    std::cout << "nothing to add" << std::endl;
    return;
}
for(auto & entry : status){
    if(entry.filename  == filename){

        if(entry.status == UNTRACKED || entry.status == MODIFIED){
            index[filename] = entry.time ;
        }
        else if(entry.status == STAGED){
            std::cout << "file already staged" << std::endl;
        }
        else if(entry.status == DELETED){
            std::cout << "file already deleted" << std::endl;
        }
        else if(entry.status == CLEAN){
            std::cout << "file already commited" << std::endl;
        }
    }
}
std::ofstream indexfile(path+"/.mygit/index");
for(auto & [file,time] : index){
    indexfile << file << " " << time << std::endl;
}

}

