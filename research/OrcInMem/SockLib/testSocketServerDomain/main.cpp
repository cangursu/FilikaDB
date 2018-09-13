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



#include "SocketServer.h"
#include "SocketClient.h"
#include "SocketDomain.h"
#include "SocketUtils.h"

#include <iostream>


template <typename TSockSrv, typename TSockCln>
class EchoServer : public SocketServer<TSockSrv, TSockCln>
{
    public:
        EchoServer(const char *name):SocketServer<TSockSrv, TSockCln>(name)
        {
        }

        virtual void OnAccept(const TSockCln &sock, const sockaddr &addr)
        {
            std::cout << "Connection Accepted  \n";
            ClientCount();
        }

        virtual void OnDisconnect  (const TSockCln &sock)
        {
            std::cout << "Client Disconnected. \n";
            ClientCount();
        }

        virtual void OnErrorClient(const TSockCln &, SocketResult res)
        {
            std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }

        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };

        void ClientCount()
        {
            std::cout << "Client count = " << SocketServer<TSockSrv, TSockCln>::ClientCount() << std::endl;
        }
};


template <typename TSocket>
class EchoClient : public SocketClient<TSocket>
{
    public:
        EchoClient(const char *name = "EchoClient")
            : SocketClient<TSocket>(name)
        {
        }
        EchoClient(int fd, const char *name)
            : SocketClient<TSocket>(fd, name)
        {
        }

        virtual void OnRecv (MemStream<std::uint8_t> &&stream)
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


        virtual void OnErrorClient (SocketResult res)
        {
            std::cout << "ErrorClient::OnErrorClient - " << SocketResultText(res) << std::endl;
        }


};


int main()
{
    EchoServer<SocketDomain, EchoClient<SocketDomain>> srv("EchoServer");
    srv.SocketPath("/home/postgres/.sock_rawtest");


    if (SocketResult::SR_ERROR == srv.Init())
    {
        std::cerr << "Unable to initialize Echo Server\n";
        return -1;
    }

    srv.LoopListen();

    return 0;
}


