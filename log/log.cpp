#include "log.h"
#include "../track/track.h"
#include <filesystem>
#include "../utils/getroot.h"



void logtree()
{
    auto path = getroot();

    auto head = loadhead(path);

    auto commit =
        loadbranch(path,head);

    print_graph(commit);
}



void log(){
{
    std::string path = getroot();

    auto head = loadhead(path);

    auto current_commit =
        loadbranch(path, head);

    std::cout
        << "========================================\n"
        << "Branch : "
        << head.substr(head.find('/') + 7)
        << '\n'
        << "HEAD   : "
        << current_commit.substr(0,7)
        << '\n'
        << "========================================\n\n";

    while(!current_commit.empty())
    {
        print_metadata(current_commit);

        current_commit =
            get_parent(current_commit);
    }
}
}








void print_metadata(std::string commit)
{
    std::string path = getroot();

    std::ifstream file(
        path + "/.mygit/objects/" +
        commit + "/metadata"
    );

    if(!file.is_open())
        return;

    std::string parent;
    std::string message;
    std::string time_string;

    std::string line;

    while(std::getline(file,line))
    {
        if(line.find("parent :") != std::string::npos)
        {
            parent = line.substr(
                line.find(':') + 1
            );

            parent.erase(
                0,
                parent.find_first_not_of(' ')
            );
        }

        else if(
            line.find("message :")
            != std::string::npos)
        {
            message = line.substr(
                line.find(':') + 1
            );

            message.erase(
                0,
                message.find_first_not_of(' ')
            );
        }

        else if(
            line.find("time :")
            != std::string::npos)
        {
            time_string = line.substr(
                line.find(':') + 1
            );

            time_string.erase(
                0,
                time_string.find_first_not_of(' ')
            );
        }
    }

    std::cout
        << "* Commit : "
        << commit.substr(0,7);

    if(commit ==
       loadbranch(getroot(),
                  loadhead(getroot())))
    {
        std::cout
            << " (HEAD -> "
            << loadhead(getroot())
                   .substr(
                       loadhead(getroot())
                           .find('/') + 7)
            << ")";
    }

    std::cout << '\n';

    if(!time_string.empty())
    {
        time_t t =
            std::stoll(time_string);

        std::string date =
            std::ctime(&t);

        date.pop_back();

        std::cout
            << "| Date    : "
            << date
            << '\n';
    }

    if(!parent.empty())
    {
        std::cout
            << "| Parent  : "
            << parent.substr(0,7)
            << '\n';
    }
    else
    {
        std::cout
            << "| Parent  : None\n";
    }

    std::cout
        << "|\n"
        << "|     "
        << message
        << '\n'
        << "|\n"
        << "----------------------------------------\n\n";
}




std::string get_parent(std::string commit){
    std::string path = getroot() ;
    std::filesystem::path metadata = std::filesystem::path(path)/".mygit/objects"/commit/"metadata";

    std::ifstream file(metadata.string());
    std::string parent;
    if(file.is_open()){
        std::string line;
        while(std::getline(file,line)){
            if(line.find("parent :")!=std::string::npos){
                parent = line.substr(line.find(':') + 1);
                parent.erase(0, parent.find_first_not_of(' '));
            }
        }
        file.close();
    }
    return parent;
}





CommitNode load_commit_info(std::string hash)
{
    CommitNode node;
    node.hash = hash;

    std::ifstream file(
            getroot()+"/.mygit/objects/"+hash+"/metadata"
            );

    std::string line;

    while(std::getline(file,line))
    {
        if(line.find("parent :") != std::string::npos)
        {
            node.parent = line.substr(line.find(':')+1);
            node.parent.erase(
                    0,
                    node.parent.find_first_not_of(' ')
                    );
        }

        if(line.find("merge_parent :") != std::string::npos)
        {
            node.merge_parent =
                line.substr(line.find(':')+1);

            node.merge_parent.erase(
                    0,
                    node.merge_parent.find_first_not_of(' ')
                    );
        }
    }

    return node;
}



void print_graph(std::string commit,int depth )
{
    if(commit.empty())
        return;

    auto node = load_commit_info(commit);

    for(int i=0;i<depth;i++)
        std::cout<<"| ";

    std::cout<<"* "<<node.hash<<'\n';

    if(!node.merge_parent.empty())
    {
        for(int i=0;i<depth;i++)
            std::cout<<"| ";

        std::cout<<"|\\\n";

        print_graph(node.merge_parent,depth+1);
    }

    print_graph(node.parent,depth);
}
