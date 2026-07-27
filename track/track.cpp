#include "track.h"
#include <filesystem>
#include <filesystem>
#include <unordered_map>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include "../utils/getroot.h"
#include "../utils/hash_file.h"
using namespace std;
namespace fs = std::filesystem;
TrackResult trackall() {
    std::string path = getroot();

    if (!fs::exists(path + "/.mygit/index")) {
        std::ofstream(path + "/.mygit/index");
    }

    auto mydir_files   = load_dir(path);
    auto myindex_files = load_index(path);

    auto head   = loadhead(path);
    auto branch = loadbranch(path, head);

    std::unordered_map<std::string, std::string> lastcommitindex;

    if (!branch.empty()) {
        lastcommitindex =
            load_parent_index(path + "/.mygit/objects/" + branch + "/index");
    }

    TrackResult result;
    FileInfo fileinfo;

    for (auto& [file, hash] : mydir_files) {

        auto index_it = myindex_files.find(file);

        // Not in index -> UNTRACKED
        if (index_it == myindex_files.end()) {
            fileinfo.status = UNTRACKED;
            fileinfo.filename = file;
            fileinfo.hash = hash;
            result.untracked.push_back(fileinfo);
            continue;
        }

        // Working tree differs from index -> MODIFIED
        if (index_it->second != hash) {
            fileinfo.status = MODIFIED;
            fileinfo.filename = file;
            fileinfo.hash =  hash;
            result.modified.push_back(fileinfo);
            continue;
        }

        auto head_it = lastcommitindex.find(file);

        // File not in HEAD or timestamps differ -> STAGED
        if (head_it == lastcommitindex.end() ||
                head_it->second!= index_it->second) {

            fileinfo.status = STAGED;
            fileinfo.filename = file;
            fileinfo.hash = hash;
            result.staged.push_back(fileinfo);
            continue;
        }

        // Otherwise -> CLEAN
        fileinfo.status = CLEAN;
        fileinfo.filename = file;
        fileinfo.hash = hash;
        result.cleaned.push_back(fileinfo);
    }

    // Deleted files
    for (auto& [file, hash] : myindex_files) {
        if (mydir_files.find(file) == mydir_files.end()) {

            fileinfo.status = DELETED;
            fileinfo.filename = file;
            fileinfo.hash =hash ;

            result.deleted.push_back(fileinfo);
        }
    }

    // Pretty output
    if (result.untracked.empty() &&
            result.modified.empty() &&
            result.staged.empty() &&
            result.deleted.empty()) {

        std::cout << "Working tree clean, nothing to commit."
            << std::endl;
    } else {
        print(result.untracked);
        print(result.modified);
        print(result.staged);
        print(result.deleted);
    }

    return result;
}
std::unordered_map<std::string,std::string> load_index(std::string path){


    std::unordered_map<std::string,std::string> index ;
    std::ifstream indexfile(path+"/.mygit/index");
    std::string filename ;
    std::string hash ;
    while(indexfile >> filename >> hash){
        index[filename] = hash;
    }
    return index ;
}

std::unordered_map<std::string,std::string> load_parent_index(std::string path){
    std::unordered_map<std::string,std::string> index ;
    std::ifstream indexfile(path);
    std::string filename ;
    std::string hash ;
    while(indexfile >> filename >> hash){
        index[filename] = hash;
    }
    return index;
}

std::unordered_map<std::string,std::string> load_dir(std::string path){
    std::unordered_map<std::string,std::string> dirfiles ;
    std::filesystem::recursive_directory_iterator mydir(path);
    struct stat file_info ;
    for(auto & entry : mydir){
        if(entry.path().string().find("/.mygit/") != std::string::npos){
            continue;
        }
        if(!entry.is_regular_file()){
            continue; }
        if  (lstat(entry.path().string().c_str(),&file_info) == -1){
            cout << "could not stat file" << entry.path().string() << endl;
        }else{
            auto relativepath = fs::relative(entry.path(),path);
            dirfiles[relativepath.string() ] = hash_file(entry.path().string());
        }

    }
    return dirfiles ;
}
std::string loadhead(std::string path){
    std::ifstream headfile(path+"/.mygit/HEAD");
    std::string filename ;
    std::getline(headfile,filename) ;
    return filename ;
}

std::string loadbranch(std::string path , std::string branch){
    std::string commit ;
    std::ifstream branchfile(path+"/.mygit/" +branch);
    std::getline(branchfile,commit) ;
    return commit ;
}

std::unordered_map<std::string,std::string> load_commit(std::string path , std::string commit){
    std::filesystem::path commitpath = path+"/.mygit/objects/"+commit ;
    std::filesystem::recursive_directory_iterator commitfiles(commitpath);

    std::unordered_map<std::string,std::string> commitfileshash ;
    struct stat file_info ;
    for(auto & entry : commitfiles){
        if(entry.path().filename() == "/.mygit"){
            continue;
        }
        if(entry.path().filename() == "metadata"){
            continue;
        }
        if(!entry.is_regular_file()){
            continue;
        }
        if  (lstat(entry.path().string().c_str(),&file_info) == -1){
            cout << "could not stat file" << entry.path().string() << endl;
        }else{
            auto relativepath = fs::relative(entry.path(),commitpath);
            commitfileshash[relativepath.string()] = hash_file(entry.path().string());
        }
    }
    return commitfileshash ;
}
void print(std::vector<FileInfo> mytrackdata ){
    if(!mytrackdata.empty()){
        if(mytrackdata[0].status == UNTRACKED){
            std::cout << "untracked: " << std::endl;
        }
        else if(mytrackdata[0].status == MODIFIED){
            std::cout << "modified:" << std::endl;
            std::cout<<endl;
        }
        else if(mytrackdata[0].status == STAGED){
            std::cout << "staged:" << std::endl;
            std::cout<<endl;
        }
        else if(mytrackdata[0].status == DELETED){
            std::cout << "deleted:" << std::endl;
            std::cout<<endl;
        }
        else if(mytrackdata[0].status == CLEAN){
            std::cout << "commited:" << std::endl;
            std::cout<<endl;
        }
        else{
            std::cout << "unknown:" << std::endl;
        }
    }
    for(auto & entry : mytrackdata){
        if(entry.status == UNTRACKED){

            std::cout << "?    " << entry.filename << "  " << entry.hash<<  std::endl;
        }else if(entry.status == MODIFIED){
            std::cout << "^    " << entry.filename << "  " << entry.hash<<  std::endl;
        }else if(entry.status == STAGED){
            std::cout << "+    " << entry.filename << "  " << entry.hash<<  std::endl;
        }else if(entry.status == DELETED){
            std::cout << "-    " << entry.filename << "  " << entry.hash <<  std::endl;
        }else if(entry.status == CLEAN){
            std::cout << "*    " << entry.filename << "  " << entry.hash<<  std::endl;
        }
    }
}

