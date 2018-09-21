/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.h
 * Author: can.gursu
 *
 * Created on 10 September 2018, 09:59
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "SocketServer.h"
//#include "SocketClient.h"
#include "SocketClientPacket.h"
#include "MemStream.h"

#include <gtest/gtest.h>

#include "deque"
#include "thread"



std::string MemStream2String(MemStream<std::uint8_t> &stream);
void        PrintPacket(const StreamPacket &packet);

extern const char *g_logSock;
extern const char *g_srvSock;
extern const char *g_srvAddr;
extern int         g_srvPort;

//

template <typename TSocket>
class SClient : public SocketClientPacket<TSocket>
{
    public:
        SClient(const char *name = "SClient"): SocketClientPacket<TSocket>(name)
        {
        }
        virtual void OnErrorClient (SocketResult err)
        {
            std::cerr << "SocketTCP(" << this->Name() << ")::Connect  ERROR - " << ErrnoText(errno) <<  "(" << errno << ") " << strerror(errno) << std::endl;
            std::cout << ".ERROR : SClient::OnErrorClient - " << SocketResultText(err) << std::endl;
        }
        virtual void OnRecvPacket  (StreamPacket &&packet)
        {
            _packetList.push_back(std::move(packet));
        }

        bool                SendPacket(const char *payload)    { SendPacket(std::move(StreamPacket(payload, strlen(payload)))); }
        bool                SendPacket(const StreamPacket &pack);
        //const StreamPacket &Packet(int idxPacket);

        std::deque <StreamPacket> _packetList;
};


template <typename TSocket>
bool SClient<TSocket>::SendPacket(const StreamPacket &pack)
{
    const StreamPacket::byte_t *b;
    std::uint32_t l = pack.Buffer(&b);

    ssize_t res = (l > 0) ? this->Write(b, l) : 0;
    return ((res > 0) && (res == (ssize_t)l));
}


template <typename TSocket>
class PacketEchoClient : public SocketClientPacket<TSocket>
{
    public:
        PacketEchoClient(const char *name = "PacketEchoClient")
            : SocketClientPacket<TSocket>(name)
        {
        }
        PacketEchoClient(int fd, const char *name)
            : SocketClientPacket<TSocket>(fd, name)
        {
        }
        virtual void OnRecvPacket(StreamPacket &&packet)
        {
            SocketResult res = this->SendPacket(packet);  //Send Back
            _packetList.push_back(std::move(packet));
        }
        virtual void OnErrorClient (SocketResult res)
        {
            _countError++;
            LOG_LINE_GLOBAL("PacketEchoClient", "ERROR : PacketEchoClient::OnErrorClient (", _countError, ") -> ", SocketResultText(res));
        }

        std::deque<StreamPacket> _packetList;
        int                      _countError = 0;
};


template <typename TSocketServer, typename TSocketClient>
class PacketEchoServer : public SocketServer<TSocketServer, PacketEchoClient<TSocketClient>>
{
    public:
        PacketEchoServer()
            : SocketServer<TSocketServer, PacketEchoClient<TSocketClient>>("ServerEcho")
        {
        }
        virtual void OnAccept(const PacketEchoClient<TSocketClient> &sock, const sockaddr &addr)
        {
            std::string host, serv;
            if (true == NameInfo(addr, host, serv))
            {
                LOG_LINE_GLOBAL("ServerEcho", "Accepted connection on host = ", host, " port = ", serv);
                ClientCountMsg();
            }
        }

        virtual void OnDisconnect  (const PacketEchoClient<TSocketClient> &sock)
        {
            LOG_LINE_GLOBAL("ServerEcho", "Client Disconnected.");
            ClientCountMsg();
        }

        virtual void OnErrorClient(const PacketEchoClient<TSocketClient> &, SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
            LOG_LINE_GLOBAL("ServerEcho", "ErrorClient : ", SocketResultText(res));
        }
        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
            LOG_LINE_GLOBAL("ServerEcho", "ErrorServer : ", SocketResultText(res));
        };

        void ClientCountMsg()
        {
            LOG_LINE_GLOBAL("ServerEcho", "Client count = ", this->ClientCount());
        }
};


template <typename TServer, typename TClient, typename TTsetFunctor>
void ClientServerFrame(TServer &server, TClient &client, TTsetFunctor ftor)
{
    LogLineGlbSocketName(g_logSock);

    //Prepare Server
    ASSERT_EQ(SocketResult::SR_SUCCESS, server.Init());
    std::thread thSrv(   [&server](){server.LoopListen();}   );

    //Prepare Client
    EXPECT_EQ(SocketResult::SR_SUCCESS, client.Init());
    EXPECT_EQ(SocketResult::SR_SUCCESS, client.Connect());
    msleep(1);
    EXPECT_EQ(1, server.ClientCount());

    std::thread thCln ( [&client] () { client.LoopRead();} );
    msleep(1);


    ftor(client);


    client.LoopReadStop();
    if (thCln.joinable()) thCln.join();

    server.LoopListenStop();
    if (thSrv.joinable()) thSrv.join();
}



template <typename TServer, typename TClient, typename TTsetFunctor>
void ClientServerFrameMClient(TServer &server, std::vector<TClient> &clientList, TTsetFunctor f)
{
    LogLineGlbSocketName(g_logSock);

    //Prepare Server
    ASSERT_EQ(SocketResult::SR_SUCCESS, server.Init());
    std::thread thSrv(   [&server](){server.LoopListen();}   );
    msleep(2);

    //Prepare Client
    std::vector<std::thread> thClientList(clientList.size());
    for (auto &client : clientList)
    {
        EXPECT_EQ(SocketResult::SR_SUCCESS, client.Init());
        EXPECT_EQ(SocketResult::SR_SUCCESS, client.Connect());
        msleep(2);

        thClientList.push_back(std::thread( [&client] () { client.LoopRead();} ));
        msleep(1);
    }
    msleep(35);
    EXPECT_EQ(clientList.size(), server.ClientCount());


    f(clientList);

    for (auto &client : clientList)
        client.LoopReadStop();
    server.LoopListenStop();

    for (auto &thCln : thClientList)
        if (thCln.joinable())
            thCln.join();
    if (thSrv.joinable()) thSrv.join();
}


#endif /* __MAIN_H__ */

