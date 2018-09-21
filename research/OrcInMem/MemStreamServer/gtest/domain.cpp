/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "main.h"
#include "SocketClientPacket.h"
#include "SocketDomain.h"
#include "SocketServer.h"

#include <gtest/gtest.h>
#include <deque>
#include <thread>





TEST(MemStreamServerDomain, Individual)
{
    PacketEchoServer<SocketDomain, SocketDomain>    server;
    SClient<SocketDomain>                           client;

    server.SocketPath(g_srvSock);
    client.SocketPath(g_srvSock);



    std::string dataPrefix("TestDataIndividual");

    int count = 300;
    std::vector<StreamPacket> data(count);

    for (int i = 0; i < count; ++i)
    {
        std::string  payload = dataPrefix + ":" + std::to_string(i);
        data[i] = StreamPacket(payload.c_str(), payload.size());
    }


    ClientServerFrame(server, client, [&data] (auto &client)
        {
            for (const auto &item : data)
            {
                EXPECT_TRUE(client.SendPacket(item));
                //msleep(1);
            }
            msleep(50);
        }
    );


    const PacketEchoClient<SocketDomain> *cln = server.ClientByIdx(0);
    EXPECT_TRUE(cln != nullptr);
    EXPECT_EQ(count, cln->_packetList.size());

    if(cln)
    {
        int idx = 0;
        for (const auto &item : data)
        {
            const StreamPacket &packRecievedServer = cln->_packetList[idx];

            EXPECT_TRUE(packRecievedServer.Check());
            EXPECT_TRUE(packRecievedServer == item);

            const StreamPacket &packRecievedBack = client._packetList[idx];
            EXPECT_TRUE(packRecievedBack.Check());
            EXPECT_TRUE(packRecievedBack == item);

            ++idx;
        }
    }
}


TEST(MemStreamServerDomain, BulkIndividual)
{
    PacketEchoServer<SocketDomain,SocketDomain> server;
    SClient<SocketDomain>                       client;

    server.SocketPath(g_srvSock);
    client.SocketPath(g_srvSock);


    int         count   = 5;
    std::string prefix = "BulkIndividual-";

    ClientServerFrame(server, client, [&count, &prefix] (auto &client)
        {
            StreamPacket::byte_t  buff2[4096];
            StreamPacket::byte_t *pBuff = nullptr;

            int     len     = 0;
            int     lenPck  = 0;

            for (int i = 0; i < count; ++i)
            {
                std::string data = prefix + std::to_string(i);
                StreamPacket packet(data.c_str(), data.size());

                lenPck = packet.Buffer(&pBuff);
                std::memcpy(buff2 + len, pBuff, lenPck);
                len += lenPck;
            }

            ASSERT_GE(len, 0);
            ssize_t res = client.Write(buff2, len);
            ASSERT_TRUE(res > 0);
            ASSERT_TRUE(res == (ssize_t)len);

            msleep(200);
        }
    );


    const PacketEchoClient<SocketDomain> *cln = server.ClientByIdx(0);
    EXPECT_TRUE(cln != nullptr);
    EXPECT_EQ(count, cln->_packetList.size());

    if(cln)
    {
        for (int idx = 0; idx < count; ++idx)
        {
            std::string data = prefix +  std::to_string(idx);
            StreamPacket item(data.c_str(), data.size());

            const StreamPacket &packRecievedServer = cln->_packetList[idx];
            EXPECT_TRUE(packRecievedServer.Check());
            EXPECT_TRUE(packRecievedServer == item);

            const StreamPacket &packRecievedBack = client._packetList[idx];
            EXPECT_TRUE(packRecievedBack.Check());
            EXPECT_TRUE(packRecievedBack == item);
        }
    }
}


TEST(MemStreamServerDomain, Dirt)
{
    PacketEchoServer<SocketDomain,SocketDomain> server;
    SClient<SocketDomain>                       client;

    server.SocketPath(g_srvSock);
    client.SocketPath(g_srvSock);


    const char *pData = "TestDataDirt:abcdefghijklmnopqrstuvwxyz";

    ClientServerFrame(server, client, [&pData] (auto &client)
        {
            StreamPacket pck(pData);

            StreamPacket::byte_t *bufferPrt;
            int lenBuffer = pck.Buffer(&bufferPrt);

            // ERROR Message
            StreamPacket::byte_t data[lenBuffer + 16];
            data[0] = 'A';
            data[1] = 'B';
            data[2] = 'C';
            data[3] = 'D';
            std::memcpy(data+4, bufferPrt, lenBuffer);
            data[4 + lenBuffer + 1] = 'E';
            data[4 + lenBuffer + 2] = 'F';
            data[4 + lenBuffer + 3] = 'G';
            data[4 + lenBuffer + 4] = 'H';

            ssize_t res = client.Write(data, lenBuffer+8);
            ASSERT_TRUE(res > 0);
            ASSERT_TRUE(res == (ssize_t)(lenBuffer+8));
            msleep(50);
        }
    );


    const PacketEchoClient<SocketDomain> *cln = server.ClientByIdx(0);
    EXPECT_TRUE(cln != nullptr);
    EXPECT_EQ(1, cln->_packetList.size());
    EXPECT_EQ(8, cln->_countError);

    StreamPacket pckCopy(pData);

    const StreamPacket &packRecievedServer = cln->_packetList[0];
    EXPECT_TRUE(packRecievedServer.Check());
    EXPECT_TRUE(packRecievedServer == pckCopy);

    const StreamPacket &packRecievedBack = client._packetList[0];
    EXPECT_TRUE(packRecievedBack.Check());
    EXPECT_TRUE(packRecievedBack == pckCopy);
}


TEST(MemStreamServerDomain, Corrupt)
{
    int         lenBuffer   = 0;
    const char *pload       = "abcdefghijklmnopqrstuvwxyz";

    PacketEchoServer<SocketDomain,SocketDomain> server;
    SClient<SocketDomain>                       client;

    server.SocketPath(g_srvSock);
    client.SocketPath(g_srvSock);


    ClientServerFrame(server, client, [&lenBuffer, &pload](auto &client)
        {
            StreamPacket pck(pload);

            StreamPacket::byte_t *pbuff;
            lenBuffer = pck.Buffer(&pbuff);

            pbuff[14] = '.';  //Corrupting buffer...

            EXPECT_TRUE(client.SendPacket(pck));
        }
    );



    const PacketEchoClient<SocketDomain> *cln = server.ClientByIdx(0);
    EXPECT_TRUE(cln != nullptr);
    ASSERT_EQ(0, cln->_packetList.size());
    EXPECT_EQ(lenBuffer - 1, cln->_countError);
}


TEST(MemStreamServerDomain, BulkCorrupt)
{
    int count = 20;

    PacketEchoServer<SocketDomain,SocketDomain> server;
    SClient<SocketDomain>                       client;

    server.SocketPath(g_srvSock);
    client.SocketPath(g_srvSock);


    ClientServerFrame(server, client, [&count](auto &client)
        {
            StreamPacket::byte_t  buff2[4096];
            StreamPacket::byte_t *pBuff = nullptr;

            std::string           prefix = "Corrupt-";

            int  len    = 0;
            int  lenPck = 0;

            for (int i = 0; i < count; ++i)
            {
                std::string data = prefix +  std::to_string(i);
                StreamPacket packet(data.c_str(), data.size());

                lenPck = packet.Buffer(&pBuff);
                pBuff[16] = 'X';
                std::memcpy(buff2 + len, pBuff, lenPck);
                len += lenPck;
            }
            client.Send(buff2, len);
        }
    );


    const PacketEchoClient<SocketDomain> *cln = server.ClientByIdx(0);
    ASSERT_TRUE(cln != nullptr);
    EXPECT_EQ(0, cln->_packetList.size());
    EXPECT_EQ(0, client._packetList.size());
}



