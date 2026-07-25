#include "./init/init.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "track/track.h"
#include <filesystem>
int main(int argc, char *argv[])
{
    std::string path ;
    if (argc ==  2){
        if (std::string(argv[1])== "init"){
      try{path = init();
       std::cout << "directory created at " << path << std::endl; }

      catch (const std::filesystem::filesystem_error & e ) {
          std::cerr<<e.what() << '\n' ;
        }}else if(std::string(argv[1])== "status"){
            try{
                if(path.empty()){
                    path = std::filesystem::current_path().string();
                }
                auto mytrackdata = trackall(path);

            }catch (const std::filesystem::filesystem_error & e ) {
                std::cerr<<e.what() << '\n' ;
            }
    }}
    return 0;
}
