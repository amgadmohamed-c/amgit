#include <filesystem>
#include "init.h"
using namespace std;
namespace fs = std::filesystem;
void init::createdir(){
    if(fs::exists(path+"/.git")){
        printf("directory already exists");
        return ;
    }
    fs::path gitpath = path + "/.git";


    std::filesystem::create_directory(gitpath);
    std::filesystem::create_directory(path+"/.git/refs/heads");
    std::filesystem::create_directory(path+"/.git/refs/tags");
    std::filesystem::create_directory(path+"/.git/objects");
    std::filesystem::create_directory(path+"/.git/info");
    std::filesystem::create_directory(path+"/.git/logs");
}
