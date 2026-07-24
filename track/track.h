#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "state.h"
std::vector<FileInfo > trackall(std::string path);

std::unordered_map<std::string,time_t> load_index(std::string path);

std::unordered_map<std::string,time_t> load_dir(std::string path);

std::unordered_map<std::string,time_t > compareworkingwithdir(std::unordered_map<std::string,time_t> index, std::unordered_map<std::string,time_t> dir) ;

std::string loadhead(std::string path);


std::string loadbranch(std::string path , std::string branch);

std::unordered_map<std::string,time_t> load_commit(std::string path , std::string commit) ;
