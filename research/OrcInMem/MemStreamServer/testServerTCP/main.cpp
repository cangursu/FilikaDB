/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: can.gursu
 *
 * Created on 16 July 2018
 */


#include "SocketServerPacket.h"
#include "SocketClientPacket.h"
#include "SocketTCP.h"
#include "LoggerGlobal.h"


#include <iostream>
#include <thread>


class PacketEchoClient  : public  SocketClientPacket<SocketTCP>
{
    public:
        PacketEchoClient(const char *name = "PacketEchoClient" )
            : SocketClientPacket(name)
        {
        }
        PacketEchoClient(int fd, const char *name)
            : SocketClientPacket(fd, name)
        {
        }
        virtual void OnRecvPacket(StreamPacket &&packet)
        {
            DisplayPacket(packet);

            SocketResult res = SendPacket(packet);  //Echoing packet
            std::cout << "Reflecting back -> res = " << SocketResultText(res) << std::endl;
        }

        void DisplayPacket(StreamPacket packet)
        {
            msize_t         pyLen = packet.PayloadLen();
            const msize_t   buffLen = 128;
            byte_t          buff [buffLen];

            for (msize_t i = 0; i < pyLen; i += buffLen)
            {
                if (packet.PayloadPart(buff, buffLen, i) > 0)
                {
                    LOG_LINE_GLOBAL("ServerEcho", "Packet:", std::string((char*)buff, pyLen));
                    std::cout << "Packet Reveived -> " << std::string((char*)buff, pyLen) << std::endl;
                }
            }
        }
};


class PacketEchoServer : public SocketServer<SocketTCP, PacketEchoClient>
{
    public:
        PacketEchoServer()
            : SocketServer<SocketTCP, PacketEchoClient>("ServerEcho")
        {
        }
        virtual void OnAccept(const PacketEchoClient &sock, const sockaddr &addr)
        {
            std::string host, serv;
            if (true == NameInfo(addr, host, serv))
            {
                LOG_LINE_GLOBAL("ServerEcho", "Accepted connection on host = ", host, " port = ", serv);
                ClientCount();
            }
        }
        virtual void OnRecv(PacketEchoClient &sock, MemStream<uint8_t> &&stream)
        {
            LOG_LINE_GLOBAL("ServerEcho", "PacketEchoServer : OnRecv - lenght:", stream.Len());
            sock.OnRecv(std::move(stream));
        }

        virtual void OnDisconnect  (const PacketEchoClient &sock)
        {
            LOG_LINE_GLOBAL("ServerEcho", "Client Disconnected.");
            ClientCount();
        }

        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
            LOG_LINE_GLOBAL("ServerEcho", "ErrorClient : ", SocketResultText(res));
        }

        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
            LOG_LINE_GLOBAL("ServerEcho", "ErrorServer : ", SocketResultText(res));
        };

        void ClientCount()
        {
            LOG_LINE_GLOBAL("ServerEcho", "Client count = ", SocketServer<SocketTCP, PacketEchoClient>::ClientCount());
        }
};


/*
class SenderTestEcho : public SocketDomain
{
public:
    SenderTestEcho() : SocketDomain("SenderTest")
    {
    }
    bool init(const char *sockPath)
    {
        SocketDomain::SocketPath(sockPath);
        if (SocketResult::SR_SUCCESS != Init())
        {
            std::cerr << "ERROR: Unable to init SocketDomain \n";
            return false;
        }
        if (SocketResult::SR_SUCCESS != SocketDomain::Connect())
        {
            std::cerr << "ERROR: Unable to Connect SocketDomain \n";
            return false;
        }
    }
};
*/

int main(int argc, char** argv)
{
    std::cout << "StreamServerClient v0\n";

    const char *slog = "/home/postgres/.sock_domain_log";
    const char *ssrv = "127.0.0.1";
    int         psrv = 5001;

    std::cout << "Using Log Server  : " << slog << std::endl;
    LogLineGlbSocketName(slog);
    LOG_LINE_GLOBAL("ServerEcho", "ver 0.0.0.0");



    PacketEchoServer server/*(que)*/;
    std::cout << "Using Echo Server : " << ssrv << ":" << psrv << std::endl;
    LOG_LINE_GLOBAL("ServerEcho", "Using Echo Server : ", ssrv, ":", psrv);
    //server.SocketPath(ssrv);
    server.Address(ssrv, psrv);

    if (SocketResult::SR_ERROR == server.Init())
    {
        LOG_LINE_GLOBAL("ServerEcho", "Unable to initialize Echo Server");
        std::cerr << "Unable to initialize Echo Server" << std::endl;
        return -1;
    }

    std::thread thSrv(   [&server](){server.LoopListen();}   );
    if (thSrv.joinable())    thSrv.join();

    return 0;
}




