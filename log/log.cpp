#include "log.h"
#include "../track/track.h"
#include <filesystem>
#include "../utils/getroot.h"
void log(){
    std::string path=  getroot();
    auto head = loadhead(path);
    std::cout << "head is " << head << std::endl;
    auto branch = loadbranch(path,head);
    while(!branch.empty()){
        print_metadata(branch);

        branch = get_parent(branch);
    }
}
void print_metadata(std::string commit) {
    std::string path = getroot();
    std::filesystem::path metadata =
        std::filesystem::path(path) / ".mygit/objects" / commit / "metadata";

    std::ifstream file(metadata.string());

    if (!file.is_open())
        return;

    std::cout << "* Commit " << commit << '\n';

    std::string line;
    while (std::getline(file, line)) {
        std::cout << "| " << line << '\n';
    }

    std::cout << "|\n";

    file.close();
}
std::string get_parent(std::string commit){
    std::string path = getroot() ;
    std::filesystem::path metadata = std::filesystem::path(path)/".mygit/objects"/commit/"metadata";

    std::ifstream file(metadata.string());
    std::string parent;
    if(file.is_open()){
        std::string line;
        while(std::getline(file,line)){
            if(line.find("parent :")!=std::string::npos){
                parent = line.substr(line.find(':') + 1);
                parent.erase(0, parent.find_first_not_of(' '));
            }
        }
        file.close();
    }
    return parent;
}
