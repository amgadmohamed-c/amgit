#include "add.h"
#include "../track/track.h"
#include "../utils/getroot.h"
#include "../utils/sha1.hpp"

void add(std::string filename){
std::string path = getroot();

auto index = load_index(path);
auto result = trackall();
if(result.untracked.empty() && result.modified.empty()){
    std::cout << "nothing to add" << std::endl;
    return;
}
for(auto & entry : result.untracked){
    if(entry.filename  == filename){

        if(entry.status == UNTRACKED || entry.status == MODIFIED){
            index[filename] = entry.time ;
        }}}

for(auto & entry : result.modified){
    if(entry.filename  == filename){
        if(entry.status == MODIFIED){
            index[filename] = entry.time ;
        }
    }
}

std::ofstream indexfile(path+"/.mygit/index");
for(auto & [file,time] : index){
    indexfile << file << " " << time << std::endl;
}

}

