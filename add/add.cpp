#include "add.h"
#include "../track/track.h"
#include "../utils/getroot.h"
#include "../utils/hash_file.h"
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

        if(entry.status == UNTRACKED ){
            index[filename] = entry.hash;
            cout << "new track [ " << filename << " ]" << endl;
        }}}

for(auto & entry : result.modified){
    if(entry.filename  == filename){
        if(entry.status == MODIFIED){
           std::cout << "MATCH" << std::endl;
            index[filename] = entry.hash;
            cout << "tracked after modification [ " << filename << " ]" << endl;
        }
    }
}

std::ofstream indexfile(path+"/.mygit/index");
for(auto & [file,hash] : index){
    indexfile << file << " " <<hash<< std::endl;
}

}

