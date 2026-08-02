#include "./init/init.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "track/track.h"
#include <filesystem>
#include "add/add.h"
#include "commit/commit.h"
#include "log/log.h"
#include "utils/getroot.h"
#include "branch/branch.h"
#include "./merge/merge.h"
#include "./revert/revert.h"
int main(int argc, char *argv[])
{
    std::string path = getroot();
    if (argc ==  2){
        if (std::string(argv[1])== "init"){
            try{
                init();
                std::cout << "directory created at " << path << std::endl; }

            catch (const std::filesystem::filesystem_error & e ) {
                std::cerr<<e.what() << '\n' ;
            }}else if(std::string(argv[1])== "status"){
                try{
                    auto mytrackdata = trackall();

                }catch (const std::filesystem::filesystem_error & e ) {
                    std::cerr<<e.what() << '\n' ;
                }
            }else if(std::string(argv[1])== "log"){
                try{
                    log();
                }catch (const std::filesystem::filesystem_error & e ) {
                    std::cerr<<e.what() << '\n' ;
                }
            }else if(std::string(argv[1])== "branch"){
                try{
                    branch();
                }catch (const std::filesystem::filesystem_error & e ) {
                    std::cerr<<e.what() << '\n' ;
                }
            }

    }
    else if(argc == 3){
        if(std::string(argv[1]) == "add"){
            try{
                add(argv[2]);
            }catch (const std::filesystem::filesystem_error & e ) {
                std::cerr<<e.what() << '\n' ;
            }
        }else if(std::string(argv[1]) == "commit"){
            try{
                commit(argv[2]);
            }catch (const std::filesystem::filesystem_error & e ) {
                std::cerr<<e.what() << '\n' ;
            }
        }else if(std::string(argv[1]) == "branch"){
            try{
                createbranch(argv[2]) ;
            }catch(const std::filesystem::filesystem_error & e){
                std::cerr<<e.what() << '\n' ;
            }}else if(std::string(argv[1]) == "checkout"){
                try{
                    checkout(argv[2]);
                }catch(const std::filesystem::filesystem_error & e){
                    std::cerr<<e.what() << '\n' ;
                }
            }else if(std::string(argv[1]) == "merge"){
                try{
                    merge(argv[2]);
                }catch(const std::filesystem::filesystem_error & e){
                    std::cerr<<e.what() << '\n' ;
                } }else if(std::string(argv[1]) == "revert"){
                try{
                    revert(argv[2]);
                }catch(const std::filesystem::filesystem_error & e){
                    std::cerr<<e.what() << '\n' ;
                } }}
    return 0;
}
