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


#include "SourceChannel.h"
#include "SocketServer.h"
#include "LoggerGlobal.h"


#include <iostream>
#include <thread>



class PacketServer : public SocketServer<SocketDomain, SourceChannelClient>
{
    public:
        PacketServer() : SocketServer<SocketDomain, SourceChannelClient>("PacketServer")
        {
        }

        virtual void OnAccept(const SourceChannelClient &sock, const sockaddr &addr)
        {
            std::string host, serv;
            if (true == NameInfo(addr, host, serv))
            {
                LOG_LINE_GLOBAL("SServerClient", "Accepted connection on host = ", host, " port = ", serv);
                ClientCount();
            }
        }

        virtual void OnRecv(/*const*/ SourceChannelClient &sock, MemStream<uint8_t> &&stream)
        {
            StreamPacket packet;

            msize_t offsetStream = 0L;
            msize_t offsetPacket = 0L;
            msize_t stLen        = stream.Len();
            msize_t pyLen        = 0L;
            msize_t pkLen        = 0L;

            const msize_t        buffLen = 128;
            StreamPacket::byte_t buff [buffLen];

            LOG_LINE_GLOBAL("SServerClient");

            SocketResult res = SocketResult::SR_ERROR_AGAIN;
            while(SocketResult::SR_ERROR_AGAIN == res)
            {
                auto reader = [&stream, &offsetStream] (char *buff, int len) -> int { return stream.read(buff, len, offsetStream); };
                res = sock.recvPacket(packet, reader);
                //res = sock.recvPacket(packet, stream, offsetStream);


                //LOG_LINE_GLOBAL("SServerClient", "recvPAcket : ", SocketResultText(res));
                if (res == SocketResult::SR_ERROR_AGAIN || res == SocketResult::SR_SUCCESS)
                {
                    pkLen = packet.BufferLen();
                    offsetStream += pkLen;

                    pyLen = packet.PayloadLen();
                    //LOG_LINE_GLOBAL("SServerClient", "stLen:", stLen, " pkLen:", pkLen, ", pyLen:", pyLen, ", offsetStream:", offsetStream);

                    for (msize_t i = 0; i < pyLen; i += buffLen)
                    {
                        if (packet.PayloadPart(buff, buffLen, i) > 0)
                        {
                            LOG_LINE_GLOBAL("SServerClient", "Packet:", std::string((char*)buff, pyLen));
                        }
                    }
                }
            }
        }

        virtual void OnDisconnect  (const SourceChannelClient &sock)
        {
            LOG_LINE_GLOBAL("SServerClient", "Client Disconnected.");
            ClientCount();
        }

        virtual void OnErrorClient(SocketResult res)
        {
            LOG_LINE_GLOBAL("SServerClient", "ErrorClient : ", SocketResultText(res));
        }

        virtual void OnErrorServer(SocketResult res)
        {
            LOG_LINE_GLOBAL("SServerClient", "ErrorServer : ", SocketResultText(res));
        };

        void ClientCount()
        {
            LOG_LINE_GLOBAL("SServerClient", "Client count = ", SocketServer<SocketDomain, SourceChannelClient>::ClientCount());
        }
};




int main(int argc, char** argv)
{
    std::cout << "StreamServerClient v0\n";

    const char *slog = "/home/postgres/.sock_memserverlog";
    std::cout << "Using LogServer : " << slog << std::endl;
    LogLineGlbSocketName(slog);
    LOG_LINE_GLOBAL("SServerClient", "ver 0.0.0.0");



    PacketServer srv;
    srv.SocketPath("/home/postgres/.sock_pgext_domain");


    if (SocketResult::SR_ERROR == srv.Init())
    {
        perror("Unable to initialize Echo Server");
    }

    srv.ListenLoop();

    return 0;


    return 0;
}




