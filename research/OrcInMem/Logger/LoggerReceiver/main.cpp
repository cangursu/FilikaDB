
#include <iostream>
#include "Logger.h"

#define SOCK_PATH_DEFAULT "/home/postgres/sock_orcinmem"

int main(int argc, char** argv)
{
    const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "Filika Logger Receiver Entered : " << sfile << std::endl;

    Filika::LSockLogServer  lg(sfile);
    std::string logitem;
    while (true)
    {
        lg.recv(logitem);
        std::cout << logitem << std::endl;
    }

    std::cout << "Filika Logger Receiver Quited\n";
    return 0;
}


