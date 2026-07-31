
#include <string>
#include  <vector>
#include <unordered_map>
void merge(std::string upcomingbranch);

std::vector<std::string> read_file(std::string path);

enum MergeResult
{
    TAKEINCOMING,
    TAKECURRENT,

};
enum MergeLineResult
{
    AUTO_MERGED,
    CONFLICT
};

MergeLineResult merge_file(
    const std::string& ancestor,
    const std::string& current,
    const std::string& incoming,
    const std::string& output);

void save_index(
    const std::string& root,
    const std::unordered_map<
        std::string,
        std::string>& index);

void rebuild_index_from_worktree();
