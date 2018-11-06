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

#include "OrcRemoteServer.h"
#include "OrcRemoteServerClient.h"
#include "SocketTCP.h"

#include <gtest/gtest.h>
#include <thread>
#include <list>

/*
#include "SocketServer.h"
//#include "SocketClient.h"
#include "SocketClientPacket.h"
#include "MemStream.h"

#include <gtest/gtest.h>

#include "deque"

*/


std::string MemStream2String(MemStream<std::uint8_t> &stream);
long        FileSize(const char *fname);
bool        CompareFile(const char *f1, const char *f2);
bool        CompareFile(FILE* fp1, FILE* fp2);

void        PrintPacket(const StreamPacket &packet);

extern const char *g_logSock;
extern const char *g_srvSock;
extern const char *g_srvAddr;
extern int         g_srvPort;



class MyTestClient : public SocketClientPacket<SocketTCP>
{
    public:
        MyTestClient(const char *name) : SocketClientPacket<SocketTCP>(name)
        {
        }

        virtual void OnErrorClient (SocketResult err)
        {
            std::cerr << "SocketClientPacket::OnErrorClient ->" << SocketResultText(err) << std::endl;
        }

        virtual void OnRecvPacket(StreamPacket &&packet)
        {
            std::vector <MemStreamPacket::Cmd> cmds;
            MemStreamPacket stream;
            int count = stream.DecodePacket(packet, cmds);

//;            _receivedCmds.reserve(_receivedCmds.size() + cmds.size());
            std::move(std::begin(cmds), std::end(cmds), std::back_inserter(_receivedCmds));

            //DisplayPacket("Recieved Packet", packet);
            //for (const auto &cmd : cmds)
            //    std::cout << cmd.Dump() << std::endl;
        }

        std::string SendPacketWrite(const char *source, const char *content)
        {
            return SendPacketWrite(source, content, std::strlen(content));
        }

        std::string SendPacketWrite(const char *source, const void *content, size_t length)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            std::string refid = packet.CreatePacketWrite("MyTestClient", source, length, content);

            if (refid.size() > 0)
                res = this->SendPacket(packet);
            if (res != SocketResult::SR_SUCCESS)
                refid.clear();
            return refid;
        }

        std::string SendPacketRead(const char *source, size_t length, size_t offset)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            std::string refid = packet.CreatePacketRead("MyTestClient", source, length, offset);

            if (refid.size() > 0)
                res = this->SendPacket(packet);
            if (res != SocketResult::SR_SUCCESS)
                refid.clear();
            return refid;
        }

        std::string SendPacketLength(const char *source)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            std::string refid = packet.CreatePacketLength("MyTestClient", source);

            if (refid.size() > 0)
                res = this->SendPacket(packet);
            if (res != SocketResult::SR_SUCCESS)
                refid.clear();
            return refid;
        }

        std::string SendPacketDelete(const char *source)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            std::string refid = packet.CreatePacketDelete("MyTestClient", source);

            if (refid.size() > 0)
                res = this->SendPacket(packet);
            if (res != SocketResult::SR_SUCCESS)
                refid.clear();
            return refid;
        }

        void DisplayPacket(const char *msg, StreamPacket &packet)
        {
            msize_t         pyLen     = packet.PayloadLen();
            msize_t         pyLenRead = 0;
            const msize_t   buffLen   = 32;
            byte_t          buff [buffLen];

            std::stringstream ss;

            for (msize_t i = 0; i < pyLen; i += buffLen)
            {
                if ((pyLenRead = packet.PayloadPart(buff, buffLen, i)) > 0)
                    ss << std::string((char*)buff, pyLenRead);
            }

            std::cout << "ORC Remote Client  - " << msg << " : \n" << ss.str() << std::endl;
        }


        std::list <MemStreamPacket::Cmd>                       _receivedCmds;
        std::unordered_map<std::string, MemStreamPacket::Cmd*> _refIdIndex;
};





template <typename TServerSCK, typename TSClientSCK, typename TTsetFunctor>
void ClientServerFrame(TTsetFunctor ftor)
{
    //OrcRemoteServer<SocketTCP, OrcRemoteServerClient> server("OrcRemoteServer");
    OrcRemoteServer<TServerSCK, TSClientSCK> server("OrcRemoteServer");
    server.Address("127.0.0.1", 5050);
    ASSERT_EQ(SocketResult::SR_SUCCESS, server.Init());
    std::thread thSrv(   [&server](){server.LoopListen();}   );

    MyTestClient client ("TestClient");
    client.Address("127.0.0.1", 5050);
    EXPECT_EQ(SocketResult::SR_SUCCESS, client.Init());
    EXPECT_EQ(SocketResult::SR_SUCCESS, client.ConnectServer());
    msleep(1);
    EXPECT_EQ(1, server.ClientCount());

    std::thread thCln ( [&client] () { client.LoopStart();} );

    //-----------------------
    ftor(client);
    //-----------------------

    client.LoopStop();
    if (thCln.joinable()) thCln.join();

    server.LoopListenStop();
    if (thSrv.joinable()) thSrv.join();

}

#endif /* __MAIN_H__ */

