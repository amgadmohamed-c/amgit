using namespace std;
#include <vector>
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
    std::string hash ;
};
struct TrackResult{
    std::vector<FileInfo> untracked;
    std::vector<FileInfo> modified;
    std::vector<FileInfo> staged;
    std::vector<FileInfo> deleted;
    std::vector<FileInfo> cleaned;
};
