#include "getroot.h"
#include <filesystem>
#include <string>
std::string getroot(){
    std::filesystem::path path = std::filesystem::current_path();
    while(path.string()!="/"){
        if(std::filesystem::exists(path.string()+"/.mygit")){
            return path.string();
        }
        path = path.parent_path();
    }
    if  (std::filesystem::exists(path.string()+"/.mygit")){
        return path.string();
    }
    return "" ;
}

