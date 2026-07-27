#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "state.h"
TrackResult trackall();

std::unordered_map<std::string,std::string> load_index(std::string path);

std::unordered_map<std::string,std::string> load_parent_index(std::string path);

std::unordered_map<std::string,std::string> load_dir(std::string path);


std::string loadhead(std::string path);


std::string loadbranch(std::string path , std::string branch);

std::unordered_map<std::string,std::string > load_commit(std::string path , std::string commit) ;

void print(std::vector<FileInfo> mytrackdata);
