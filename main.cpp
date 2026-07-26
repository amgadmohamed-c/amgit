#include "./init/init.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "track/track.h"
#include <filesystem>
#include "add/add.h"
#include "commit/commit.h"
#include "log/log.h"
int main(int argc, char *argv[])
{
    std::string path = std::filesystem::current_path().string();
    if (argc ==  2){
        if (std::string(argv[1])== "init"){
            try{init();
                std::cout << "directory created at " << path << std::endl; }

            catch (const std::filesystem::filesystem_error & e ) {
                std::cerr<<e.what() << '\n' ;
            }}else if(std::string(argv[1])== "status"){
                try{
                    auto mytrackdata = trackall(path);

                }catch (const std::filesystem::filesystem_error & e ) {
                    std::cerr<<e.what() << '\n' ;
                }
            }else if(std::string(argv[1])== "log"){
                try{
                    log();
                }catch (const std::filesystem::filesystem_error & e ) {
                    std::cerr<<e.what() << '\n' ;
                }
            }

    }
    else if(argc == 3){
        if(std::string(argv[1]) == "add"){
            try{
                add(path,argv[2]);
                std::cout << "file added to index" <<argv[2] << std::endl;
            }catch (const std::filesystem::filesystem_error & e ) {
                std::cerr<<e.what() << '\n' ;
            }
        }else if(std::string(argv[1]) == "commit"){
            try{
                commit(argv[2]);
            }catch (const std::filesystem::filesystem_error & e ) {
                std::cerr<<e.what() << '\n' ;
            }
        }
    }
    return 0;
}
