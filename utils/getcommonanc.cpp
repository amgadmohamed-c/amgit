#include "../track/track.h"
#include "./getroot.h"
#include "getcommonanc.h"
#include "../log/log.h"
#include <set>
std::string getcommonanc(std::string upcomingbranch){
    std::string root = getroot();
    std::string head = loadhead(root);
    std::string currbranch= loadbranch(root , head);
    std::string upbranch = loadbranch(root ,"refs/heads/"+upcomingbranch);


    if(currbranch==upbranch){

        return "";
    }
    std::set<std::string> currcommit;

    while(!currbranch.empty()){
        currcommit.insert(currbranch);
        currbranch = get_parent(currbranch);
    }

    while(!upbranch.empty()){
        if(currcommit.count(upbranch)){
            return upbranch;
        }
        upbranch = get_parent(upbranch);

    }

return "" ;


}
