/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: can.gursu
 *
 * Created on 10 July 2018, 09:33
 */



/*
 * =====================================================================================
 *
 *        http://www.voidcn.com/article/p-uvhjqbmi-uo.html
 *
 * =====================================================================================
 */



#include "Socket.h"
#include "SocketUtils.h"

#include <iostream>



class EchoServer : public SocketServer
{
    public:

        virtual void OnAccept(const Socket &sock, const sockaddr &addr)
        {
            std::string host, serv;
            if (SocketResult::SUCCESS == sock.NameInfo(addr, host, serv))
            {
                std::cout << "Accepted connection on host = " << host  << " port = " << serv << std::endl;
                std::cout << "Client count = " << ClientCount() << std::endl;
            }
        }
        virtual void OnRecv(const Socket &sock, MemStream<uint8_t> &&stream)
        {
            const int buffLen = 128;
            char buff[buffLen];

            for (std::uint8_t i = 0, readed = 0, len = stream.Len(); i < len; i += readed)
            {
                readed = stream.read(buff, buffLen-1, i);
                buff[readed] = '\0';
                std::cout << buff;
            }
            std::cout << std::endl;
        }
        virtual void OnDisconnect  (const Socket &sock)
        {
            std::cout << "Disconnected Client.  " << ClientCount() << std::endl;
        }
        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }
        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };
};


int main()
{
    EchoServer srv;
    if (SocketResult::ERROR == srv.Init())
    {
        perror("Unable to initialize Echo Server");
    }

    srv.ListenLoop();
}


