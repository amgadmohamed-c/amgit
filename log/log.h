


#include <string>

struct CommitNode {
    std::string hash;
    std::string parent;
    std::string merge_parent;
};

void log();
void logtree();
void print_metadata(std::string commit);
std::string get_parent(std::string commit);

CommitNode load_commit_info(std::string hash) ;


void print_graph(std::string commit,int depth = 0 );

