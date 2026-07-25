#include "track.h"
#include <filesystem>
#include <filesystem>
#include <unordered_map>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
using namespace std;
namespace fs = std::filesystem;
std::vector<FileInfo > trackall(std::string path){
    std::vector<FileInfo> mytrackdata ;
    FileInfo fileinfo ;
    if(!fs::exists(path+"/.mygit/index") ){
        std::ofstream index(path+"/.mygit/index");
    }


    auto mydir_files =load_dir(path);
    auto myindex_files =load_index(path) ;
    auto head = loadhead(path) ;
    auto branch = loadbranch(path,head) ;
    std::unordered_map<std::string,time_t> commit ;
    if(!branch.empty()){

        commit = load_commit(path,branch) ;
    }


    for(auto & entry : mydir_files){
        if(entry.first.find("/.mygit/") != std::string::npos)
            continue;
        auto it = myindex_files.find(entry.first) ;
        auto it2 = commit.find(entry.first) ;
        if(it == myindex_files.end()){
            fileinfo.status = UNTRACKED ;
            fileinfo.filename = entry.first ;
            fileinfo.time = entry.second ;
            mytrackdata.push_back(fileinfo);
        }
        else if(it->second != entry.second){
            fileinfo.status = MODIFIED ;
            fileinfo.filename = entry.first ;
            fileinfo.time = entry.second ;
            mytrackdata.push_back(fileinfo);
        }
        else if(it2==commit.end()){
            fileinfo.status = STAGED ;
            fileinfo.filename = entry.first ;
            fileinfo.time = entry.second ;
            mytrackdata.push_back(fileinfo);

        }

        else if(it2 != commit.end() && it2->second != entry.second){
            fileinfo.status = STAGED ;
            fileinfo.filename = entry.first ;
            fileinfo.time = entry.second ;
            mytrackdata.push_back(fileinfo);
        }else{

            fileinfo.status = CLEAN ;
            fileinfo.filename = entry.first ;
            fileinfo.time = entry.second ;
            mytrackdata.push_back(fileinfo);
        }
    }
    for(auto &[file,time] : myindex_files){
        if(mydir_files.find(file) == mydir_files.end()){
            fileinfo.status = DELETED ;
            fileinfo.filename = file ;
            fileinfo.time = time ;
            mytrackdata.push_back(fileinfo);
        }
    }
    print(mytrackdata);

    return mytrackdata;
}
std::unordered_map<std::string,time_t> load_index(std::string path){
    std::unordered_map<std::string,time_t> index ;
    std::ifstream indexfile(path+"/.mygit/index");
    std::string filename ;
    time_t time ;
    while(indexfile >> filename >> time){
        index[filename] = time ;
    }
    return index ;
}
std::unordered_map<std::string,time_t> load_dir(std::string path){
    std::unordered_map<std::string,time_t> dirfiles ;
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
            dirfiles[relativepath.string() ] = file_info.st_mtime;
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

std::unordered_map<std::string,time_t> load_commit(std::string path , std::string commit){
    std::filesystem::path commitpath = path+"/.mygit/objects/"+commit ;
    std::filesystem::recursive_directory_iterator commitfiles(commitpath);

    std::unordered_map<std::string,time_t> commitfilestimes ;
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
            commitfilestimes[relativepath.string()] = file_info.st_mtime;
        }
    }
    return commitfilestimes ;
}
 void print(std::vector<FileInfo> mytrackdata){
     for(auto & entry : mytrackdata){
        if(entry.status == UNTRACKED){
            std::cout << "new file [ " << entry.filename <<  " ]"<<std::endl;
            std::cout << "time " << entry.time << std::endl;
        }
        else if(entry.status == MODIFIED){
            std::cout << "modified file " << entry.filename << std::endl;
            std::cout << "time " << entry.time << std::endl;
        }
        else if(entry.status == STAGED){
            std::cout << "staged file " << entry.filename << std::endl;
            std::cout << "time " << entry.time << std::endl;
        }
        else if(entry.status == DELETED){
            std::cout << "deleted file " << entry.filename << std::endl;
            std::cout << "time " << entry.time << std::endl;
        }
        else if(entry.status == CLEAN){
            std::cout << "commited file " << entry.filename << std::endl;
            std::cout << "time " << entry.time << std::endl;
        }
    }
 }

