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
std::vector<std::string > trackunstaged(std::string path){
    std::vector<std::string> untrackedfiles ;
    if(!fs::exists(path+"/.mygit") ){
        printf("directory does not exist");
        return untrackedfiles;
    }
    std::unordered_map<std::string,time_t> myfiles ;
    std::filesystem::recursive_directory_iterator mydir(path);
    struct stat file_info ;
    if(!fs::exists(path+"/.mygit/index") ){
        std::ofstream index(path+"/.mygit/index");

        for(auto & entry : mydir){
            if(entry.path().filename() == ".mygit"){
                continue;
            }
            if  (lstat(entry.path().string().c_str(),&file_info) == -1){
                printf("could not stat file");
            }else{


                myfiles[entry.path().string() ] = file_info.st_mtime;
                untrackedfiles.push_back(entry.path().string());

            }

        }
        if(index.is_open()){
            for(auto & entry : myfiles){
                index << entry.first << ' ' << entry.second << '\n';
            }
            index.close();
            return untrackedfiles;
        }
        else{
            printf("could not open index");
            return untrackedfiles;
        }
    }
    else{


        std::ifstream index(path+"/.mygit/index");
        std::string filename;

        time_t stored_time ;


        while(index >> filename >> stored_time){
            myfiles[filename] = stored_time;

        }
        for(auto & entry : mydir){
            if(entry.path().string().find("/.mygit/") != std::string::npos)
                continue;

            if  (lstat(entry.path().string().c_str(),&file_info) == -1){
                continue;
            }
            std::string file = entry.path().string() ;

            auto it = myfiles.find(file) ;
            if(it == myfiles.end()){
                untrackedfiles.push_back(file);
                std::cout << "new file" << std::endl  ;
            }
            else if(it->second != file_info.st_mtime){
                untrackedfiles.push_back(file);
                std::cout << "modified file" << std::endl  ;


            }



        }
        for(auto &[file,time] : myfiles){
            if(!fs::exists(file)){
                std::cout << "deleted file" << std::endl  ;
            }
        }
    }
    return untrackedfiles;
}
