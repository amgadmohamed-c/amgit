#include <filesystem>
#include <fstream>
#include "init.h"
#include <iostream>
using namespace std;
namespace fs = std::filesystem;
std::string path = fs::current_path().string();
std::string init(){
    if(fs::exists(path+"/.mygit")){
        printf("directory already exists");
        return path ;
    }
    fs::path gitpath = path + "/.mygit";
    cout << gitpath << endl;

    std::filesystem::create_directory(gitpath);
    std::filesystem::create_directory(path+"/.mygit/refs");
    std::filesystem::create_directory(path+"/.mygit/refs/heads");
    std::filesystem::create_directory(path+"/.mygit/refs/tags");
    std::filesystem::create_directory(path+"/.mygit/objects");
    std::filesystem::create_directory(path+"/.mygit/info");
    std::filesystem::create_directory(path+"/.mygit/logs");

    fs::path HEAD = path+"/.mygit/HEAD";
    if(fs::exists(HEAD)){
        printf("HEAD already exists");
        return  path;
    }
    std::ofstream file(HEAD);
    if(file.is_open()){
        file << "ref: refs/heads/main\n";
        file.close();
    }
    else{
        printf("could not create HEAD");
        return path ;
    }
 return path;


}
