
#include <string>
#include "./hash_file.h"
#include "./sha1.hpp"
std::string hash_file(std::string filename){
    std::string hash = SHA1::from_file(filename);
    return hash;
}

