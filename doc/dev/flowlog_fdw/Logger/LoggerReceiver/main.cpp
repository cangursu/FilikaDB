
#include <iostream>
#include "Logger.h"

#define SOCK_PATH "/home/postgres/logsocket"

int main(int argc, char** argv)
{
    std::cout << "Filika Logger Receiver : " << SOCK_PATH << std::endl;
    Filika::LoggerServer  lg(SOCK_PATH);

    std::string logitem;
    while (true)
    {
        lg.recv(logitem);
        std::cout << logitem << std::endl;
    }
    return 0;
}


