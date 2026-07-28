#include "./merge.h"
#include "../utils/getcommonanc.h"
#include "../utils/getroot.h"
#include "../track/track.h"
#include "../branch/branch.h"
void merge(std::string upcomingbranch){
    std::string root = getroot();
    std::string anc = getcommonanc(upcomingbranch) ;
    std::string head = loadhead(root) ;
    std::string branch =loadbranch(root, head) ;
    std::string comingbranch = loadbranch(root, "refs/heads/"+upcomingbranch) ;
    if(anc.empty()){
        return ;
    }
    if(anc == branch){
      std::ofstream file;
      checkout(upcomingbranch);
      file.open(root+"/.mygit/"+head) ;
      file << comingbranch;
      file.close();
      file.open(root+"/.mygit/HEAD") ;
      file << head ;
      file.close();
      std::cout << "fast forward merge" << std::endl;
      return ;
    }
    if(anc == comingbranch){
      std::cout << "already up to date" << head<< std::endl;
      return ;
    }

}

