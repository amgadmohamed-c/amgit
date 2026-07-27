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
void print_metadata(std::string commit)
{
    std::string path = getroot();

    std::ifstream file(
        path + "/.mygit/objects/" + commit + "/metadata"
    );

    std::string line;
    std::string parent;
    std::string message;
    std::string time;

    while (std::getline(file, line))
    {
        if (line.find("parent :") != std::string::npos)
        {
            parent = line.substr(line.find(':') + 1);
        }
        else if (line.find("message :") != std::string::npos)
        {
            message = line.substr(line.find(':') + 1);
        }
        else if (line.find("time :") != std::string::npos)
        {
            time = line.substr(line.find(':') + 1);
        }
    }

    std::time_t t = std::stoll(time);

    std::cout << "* Commit " << commit << '\n';

    if (!parent.empty())
    {
        std::cout << "| Parent : " << parent << '\n';
    }

    std::cout << "| Date   : " << std::ctime(&t);
    std::cout << "|\n";
    std::cout << "|   " << message << '\n';
    std::cout << "|\n";
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
                parent = line.substr(line.find(":")+1)  ;
            }
        }
        file.close();
   }
 return parent;
}
