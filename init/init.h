#include <iostream>
#include <filesystem>


class init{
    private :
        int filecount ;
        int dircount ;
        std::string path ;
        std::string file[100] ;
        std::string dir[100] ;
        bool filesucess ;
        bool dirsucess ;
    public :
        init(){
          path= std::filesystem::current_path().string() ;
            filecount  =  0 ;
            dircount = 0 ;
        }
        void createdir();
        void trackfiles() ;

        void trackdirs();

        bool issuccess()
            ;
        void createbranch() ;

};
