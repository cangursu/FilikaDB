
#include "Logger.h"

#include <iostream>
#include <unistd.h>
#include <deque>
#include <functional>
#include <cstring>


#define SOCK_PATH_DEFAULT "/home/postgres/sock_orcinmem"

int Recvieve(SocketDomain &sock, std::function<void(const char *, int)>fnc)
{
    const int lenMax = 1024;
    static __thread  char buff[lenMax];

    int len = sock.Read(buff, lenMax);
    if (len > 0 && len < lenMax+1)
        fnc(buff,len);

    return len;
}


int main(int argc, char** argv)
{
    const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "Filika Logger Receiver Entered : " << sfile << std::endl;

    SocketDomain  server("LoggerReceiever");
    server.SocketPath(sfile);

    if (SocketResult::SR_SUCCESS != server.InitServer())
    {
        std::cerr << "Unable To initialize Log Server \n";
        return -1;
    }

    std::stringstream           dataPrev;
    char                        dataX[1024];
    std::string                 logitem;
    std::deque<std::string>     logList;
    while (true)
    {
        //if (!sc.Good())

        std::cout << "Accepting\n" ;
        SocketDomain sc = server.Accept();
        std::cout << "Accepted\n" ;

        int len = - 1;
        do
        {
            len = Recvieve(sc, [&dataX] (const char *b, int l)->void { std::memcpy(dataX, b, l);} );
            //std::cout << "Recvieve  len:" << len << std::endl;

            size_t p1 = 0;
            for(size_t p2 = 0; p2 < len; ++p2)
            {
                if (dataX[p2] == '\0')
                {
                    if ((p2-p1)>0)
                    {
                        std::string s (dataX+p1, p2-p1-1);
                        logList.push_back(dataPrev.str() + std::string(dataX+p1, p2-p1));

                        dataPrev.str(std::move(std::string()));
                        dataPrev.clear();
                    }
                    p1 = p2+1;
                }
            }
            if (p1 < len)
            {
                dataPrev << std::string(dataX+p1, len - p1);
                //std::cout << "dataPrev = >" << dataPrev.str() << "<" << std::endl;
            }

            for (const auto &i:logList)
                std::cout << i << std::endl;
            logList.clear();
        }  while (len > 0  );

        std::string s = dataPrev.str();
        if (s.size() > 0) std::cout << s << std::endl;
        dataPrev.str(std::move(std::string()));
        dataPrev.clear();
    }


    std::cout << "Filika Logger Receiver Quited\n";
    return 0;
}


