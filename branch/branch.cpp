#include "branch.h"
#include "../utils/getroot.h"
#include "../track/track.h"
#include "../log/log.h"
#include <filesystem>
#include <fstream>
#include <iostream>

void branch(){
    std::string path = getroot();
    std::string head = loadhead(path);

    std::cout << "your current branch is: " << head.substr(head.find("/") + 7) << std::endl;
}

void createbranch(std::string branchname){
    std::string path = getroot();
    std::string head = loadhead(path);
    std::string parent = loadbranch(path,head);


    ofstream file(path + "/.mygit/refs/heads/" + branchname);


    if (file.is_open()){
        file << parent;
        file.close();}
    std::cout << "branch created successfully" << std::endl;
}
