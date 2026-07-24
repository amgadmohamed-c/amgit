using namespace std;
#include <string>
enum state{
    TRACKED,
    UNTRACKED,
    MODIFIED,
    DELETED,
    STAGED,
    STAGED_AND_MODIFIED,
    CLEAN ,
};



struct FileInfo{
    std::string filename ;
    state status ;
    time_t time ;
};
