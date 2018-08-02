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
#include "SocketServer.h"
#include "SocketDomain.h"
#include "SocketUtils.h"

#include <iostream>


template <typename TSock>
class EchoServer : public SocketServer<TSock>
{
    public:

        virtual void OnAccept(const TSock &sock, const sockaddr &addr)
        {
            std::string host, serv;
            if (true == NameInfo(addr, host, serv))
            {
                std::cout << "Accepted connection on host = " << host  << " port = " << serv << std::endl;
                ClientCount();
            }
        }

        virtual void OnRecv(const TSock &sock, MemStream<uint8_t> &&stream)
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

        virtual void OnDisconnect  (const TSock &sock)
        {
            std::cout << "Client Disconnected. \n";
            ClientCount();
        }

        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }

        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };

        void ClientCount()
        {
            std::cout << "Client count = " << SocketServer<TSock>::ClientCount() << std::endl;
        }
};


int main()
{
    EchoServer<SocketDomain> srv;
    srv.SocketPath("/home/postgres/.sock_rawtest");


    if (SocketResult::SR_ERROR == srv.Init())
    {
        perror("Unable to initialize Echo Server");
    }

    srv.ListenLoop();

    return 0;
}


