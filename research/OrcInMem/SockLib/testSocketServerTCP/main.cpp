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



#include "SocketTCP.h"
#include "SocketServer.h"
#include "SocketClient.h"
#include "SocketDomain.h"
#include "GeneralUtils.h"

#include <iostream>


template <typename TSckSrv, typename TSckCln>
class EchoServer : public SocketServer<TSckSrv, TSckCln>
{
    public:

        EchoServer(const char *name)
            : SocketServer<TSckSrv, TSckCln>(name)
        {
        }



        virtual void OnAccept(TSckCln &sock, const sockaddr &addr)
        {
            std::string host, serv;
            if (/*SocketResult::SR_SUCCESS*/true == NameInfo(addr, host, serv))
            {
                std::cout << "Incomming connection on host = " << host  << " port = " << serv << std::endl;
                ClientCount();
            }
        }
        virtual void OnDisconnect  (const TSckCln &sock)
        {
            std::cout << "Client Disconnected. \n";
            ClientCount();
        }
        virtual void OnErrorClient(const TSckCln &, SocketResult res)
        {
            std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }
        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };

        void ClientCount()
        {
            std::cout << "Client count = " << SocketServer<TSckSrv, TSckCln>::ClientCount() << std::endl;
        }
};

class EchoClient : public SocketClient<SocketTCP>
{
    public :
        EchoClient(const char *name = "EchoClient") : SocketClient(name)
        {
        }
        EchoClient(int fd, const char *name) : SocketClient(fd, name)
        {
        }
        virtual void OnRecv (MemStream<std::uint8_t> &&stream)
        {
            const int buffLen = 128;
            char buff[buffLen];

            for (std::uint8_t i = 0, readed = 0, len = stream.Len(); i < len; i += readed)
            {
                readed = stream.Read(buff, buffLen-1, i);
                buff[readed] = '\0';
                std::cout << buff;
            }
            std::cout << std::endl;
        }
        virtual void OnErrorClient (SocketResult res)
        {
            std::cout << "EchoClient::OnErrorClient : " << SocketResultText(res) << std::endl;
        }

};

int main()
{

    EchoServer<SocketTCP, EchoClient> srv("EchoServer");

    //srv.SocketPath("/home/postgres/.sock_rawtest");
    srv.Port(5000);


    if (SocketResult::SR_ERROR == srv.Init())
    {
        std::cerr << "Unable to initialize Echo Server\n";
    }

    srv.LoopListen();
}


