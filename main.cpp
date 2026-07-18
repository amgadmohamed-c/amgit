#include "./init/init.h"
#include <string>
#include <iostream>
#include <filesystem>
int main(int argc, char *argv[])
{
    if (argc ==  2){
        if (std::string(argv[1])== "init"){
      try{init();}
      catch (const std::filesystem::filesystem_error & e ) {
          std::cerr<<e.what() << '\n' ;
        }}
    }
    return 0;
}
