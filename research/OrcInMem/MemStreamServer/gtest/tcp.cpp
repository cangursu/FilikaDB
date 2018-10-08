/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "main.h"
#include "SocketTCP.h"

#include <gtest/gtest.h>
#include <mutex>
#include <condition_variable>
#include <list>


TEST(MemStreamServerTCP, Individual)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    SClient<SocketTCP>                      client;

    server.Address("127.0.0.1", 5001);
    client.Address("127.0.0.1", 5001);

    const char *data[] =
    {
        "XXX",
        "TestDataYYY",
        "TestData_12345",
        "TestData_67890",
        "TestData_0987654321",
        "TestData_abcdefghijklmnopqrstuvwxyz"
    };

    ClientServerFrame(server, client, [&data] (auto &client)
        {
            for (const auto &item : data)
                client.SendPacket(item);
            msleep(10);
        }
    );


    const PacketEchoClient<SocketTCP> *cln = server.ClientByIdx(0);
    ASSERT_TRUE(cln != nullptr);
    ASSERT_EQ(sizeof(data)/sizeof(*data), cln->_packetList.size());

    if(cln)
    {
        int idx = 0;
        for (const auto &item : data)
        {
            StreamPacket packet(item);

            const StreamPacket &packRecievedServer = cln->_packetList[idx];

            ASSERT_TRUE(packRecievedServer.Check());
            ASSERT_TRUE(packRecievedServer == packet);

            const StreamPacket &packRecievedBack = client._packetList[idx];
            ASSERT_TRUE(packRecievedBack.Check());
            ASSERT_TRUE(packRecievedBack == packet);

            ++idx;
        }
    }
    server.Release();
}


TEST(MemStreamServerTCP, BulkIndividual)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    SClient<SocketTCP>                      client;

    server.Address("127.0.0.1", 5001);
    client.Address("127.0.0.1", 5001);

    int         count   = 100;
    std::string prefix  = "BulkIndividual-";

    ClientServerFrame(server, client, [&count, &prefix] (auto &client)
        {
            StreamPacket::byte_t    buff[count*32];
            StreamPacket::byte_t   *pBuff = nullptr;

            int  len    = 0;
            int  lenPck = 0;

            for (int i = 0; i < count; ++i)
            {
                std::string data = prefix +  std::to_string(i);
                StreamPacket packet(data.c_str(), data.size());

                lenPck = packet.Buffer(&pBuff);
                std::memcpy(buff + len, pBuff, lenPck);
                len += lenPck;
            }
            client.Write(buff, len);
            msleep(50);
        }
    );
    const PacketEchoClient<SocketTCP> *cln = server.ClientByIdx(0);
    ASSERT_TRUE(cln != nullptr);
    ASSERT_EQ(count, cln->_packetList.size());


    if(cln)
    {
        int idx = 0;
        for (int i = 0; i < count; ++i)
        {
            std::string data = prefix +  std::to_string(i);
            StreamPacket packet(data.c_str(), data.size());

            const StreamPacket &packRecievedServer = cln->_packetList[idx];

            ASSERT_TRUE(packRecievedServer.Check());
            ASSERT_TRUE(packRecievedServer == packet);

            const StreamPacket &packRecievedBack = client._packetList[idx];
            ASSERT_TRUE(packRecievedBack.Check());
            ASSERT_TRUE(packRecievedBack == packet);

            ++idx;
        }
    }


    server.Release();
}


TEST(MemStreamServerTCP, Fragmanted)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    SClient<SocketTCP>                      client;

    server.Address("127.0.0.1", 5001);
    client.Address("127.0.0.1", 5001);

    StreamPacket pck("TestDataFragmanted_1234567890_abcdefghijklmnopqrstuvwxyz");

    ClientServerFrame(server, client, [&pck] (auto &client)
        {
            StreamPacket::byte_t *  bufferPrt;
            int                     lenBuffer = pck.Buffer(&bufferPrt);

            int                     lenPart1 = lenBuffer/2;
            int                     lenPart2 = lenBuffer - lenPart1;

            StreamPacket::byte_t part1[lenPart1];
            StreamPacket::byte_t part2[lenPart2];

            std::memcpy(part1, bufferPrt, lenPart1);
//            std::cout << "Part1 \n" << dumpMemory(part1, lenPart1) << std::endl;
            std::memcpy(part2, bufferPrt+lenPart1, lenPart2);
//            std::cout << "Part2 \n" << dumpMemory(part2, lenPart2) << std::endl;


            client.Write(part1, lenPart1);
            msleep(50);
            client.Write(part2, lenPart2);
            msleep(50);
        }
    );

    const PacketEchoClient<SocketTCP> *cln = server.ClientByIdx(0);
    EXPECT_TRUE(cln != nullptr);
    if (cln)
    {
        EXPECT_EQ(1, cln->_packetList.size());
        if (1 == cln->_packetList.size())
            EXPECT_TRUE(pck == cln->_packetList[0]);
    }

    server.Release();
}



TEST(MemStreamServerTCP, FragmantedBulk)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    SClient<SocketTCP>                      client;

    server.Address("127.0.0.1", 5001);
    client.Address("127.0.0.1", 5001);

    int count  = 100;
    std::string prefix ("TestDataFragmantedBULK-");

    ClientServerFrame(server, client, [&prefix, &count] (auto &client)
        {
            for(int i = 0; i < count; i++)
            {
                StreamPacket pck( (prefix + std::to_string(i)).c_str() );

                StreamPacket::byte_t *  bufferPrt;

                int   lenBuffer = pck.Buffer(&bufferPrt);
                int   lenPart1  = lenBuffer/2;
                int   lenPart2  = lenBuffer - lenPart1;

                StreamPacket::byte_t part1[lenPart1];
                std::memcpy(part1, bufferPrt, lenPart1);
                client.Write(part1, lenPart1);
                msleep(50);

                StreamPacket::byte_t  part2[lenPart2];
                std::memcpy(part2, bufferPrt+lenPart1, lenPart2);
                client.Write(part2, lenPart2);
            }
        }
    );

    const PacketEchoClient<SocketTCP> *cln = server.ClientByIdx(0);
    ASSERT_TRUE(cln != nullptr);
    ASSERT_EQ(count, cln->_packetList.size());

    for(int i = 0; i < count; i++)
    {
        StreamPacket p( (prefix + std::to_string(i)).c_str() );
        ASSERT_TRUE( cln->_packetList[i] == p );
    }

    server.Release();
}


TEST(MemStreamServerTCP, Dirt)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    SClient<SocketTCP>                      client;

    server.Address("127.0.0.1", 5001);
    client.Address("127.0.0.1", 5001);

    std::string  prefix = std::string("Dirt-")+std::to_string(getpid());

    ClientServerFrame(server, client, [&prefix] (auto &client)
        {
            StreamPacket::byte_t  buff[4096];
            StreamPacket::byte_t *pBuff  = nullptr;


            std::string           data = prefix + "Individual";
            StreamPacket          packet(data.c_str(), data.size());

            int                   len    = 0;
            int                   lenPck = packet.Buffer(&pBuff);

            std::memcpy(buff + len, "XXX", 3);
            len+=3;

            std::memcpy(buff + len, pBuff, lenPck);
            len += lenPck;

            std::memcpy(buff + len, "YYY", 3);
            len+=3;

            client.Write(buff, len);
            msleep(1);
        }
    );

    const PacketEchoClient<SocketTCP> *cln = server.ClientByIdx(0);
    ASSERT_TRUE (cln != nullptr);
    ASSERT_EQ   (1, cln->_packetList.size());
    ASSERT_TRUE (cln->_packetList[0] == StreamPacket((prefix + "Individual").c_str()));

    server.Release();
}



TEST(MemStreamServerTCP, DirtBulk)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    SClient<SocketTCP>                      client;

    server.Address("127.0.0.1", 5001);
    client.Address("127.0.0.1", 5001);

    int         count  = 250;
    std::string prefix = "Dirt-";

    ClientServerFrame(server, client, [&prefix, &count] (auto &client)
        {
            StreamPacket::byte_t    buff[count*64];
            StreamPacket::byte_t   *pBuff   = nullptr;

            int                     len     = 0;
            int                     lenPck  = 0;

            for (int i = 0; i < count; ++i)
            {
                std::string data = prefix +  std::to_string(i);
                StreamPacket packet(data.c_str(), data.size());

                lenPck = packet.Buffer(&pBuff);

                std::memcpy(buff + len, "XXX", 3);
                len+=3;

                std::memcpy(buff + len, pBuff, lenPck);
                len += lenPck;

                std::memcpy(buff + len, "YYY", 3);
                len+=3;
            }

            std::cout << "sending  len = " << len << std::endl;
            client.Write(buff, len);
        }
    );

    const PacketEchoClient<SocketTCP> *cln = server.ClientByIdx(0);
    ASSERT_TRUE (cln != nullptr);
    ASSERT_EQ   (count, cln->_packetList.size());
    for (int i = 0; i < count; ++i)
        ASSERT_TRUE (cln->_packetList[i] == StreamPacket((prefix +  std::to_string(i)).c_str()));

    server.Release();
}


TEST(MemStreamServerTCP, Corrupt)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    SClient<SocketTCP>                      client;

    server.Address("127.0.0.1", 5001);
    client.Address("127.0.0.1", 5001);


    ClientServerFrame(server, client, [] (auto &client)
        {
            std::string             prefix = "Corrupt-Individual";
            StreamPacket::byte_t    buff[4096];
            StreamPacket::byte_t   *pBuff  = nullptr;

            int                     len    = 0;
            int                     lenPck = 0;

            std::string data = prefix;
            StreamPacket packet(data.c_str(), data.size());

            lenPck = packet.Buffer(&pBuff);
            pBuff[13] = 'X';
            std::memcpy(buff + len, pBuff, lenPck);
            len += lenPck;

            client.Write(buff, len);
        }
    );

    const PacketEchoClient<SocketTCP> *cln = server.ClientByIdx(0);
    ASSERT_TRUE (cln != nullptr);
    ASSERT_EQ   (0, cln->_packetList.size());

    server.Release();
}


TEST(MemStreamServerTCP, CorruptBulk)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    SClient<SocketTCP>                      client;

    server.Address("127.0.0.1", 5001);
    client.Address("127.0.0.1", 5001);


    ClientServerFrame(server, client, [] (auto &client)
        {
            std::string             prefix = "Corrupt-";
            StreamPacket::byte_t    buff[4096];
            StreamPacket::byte_t   *pBuff = nullptr;
            int                     len    = 0;
            int                     lenPck = 0;

            for (int i = 0; i < 1; ++i)
            {
                std::string data = prefix +  std::to_string(i);
                StreamPacket packet(data.c_str(), data.size());

                lenPck = packet.Buffer(&pBuff);
                //pBuff[16] = 'X';
                pBuff[13] = 'X';
                std::memcpy(buff + len, pBuff, lenPck);
                len += lenPck;
            }

            client.Write(buff, len);
        }
    );

    const PacketEchoClient<SocketTCP> *cln = server.ClientByIdx(0);
    ASSERT_TRUE (cln != nullptr);
    ASSERT_EQ   (0, cln->_packetList.size());

    server.Release();
}


TEST(MemStreamServerTCP, IndividualMClient)
{
    PacketEchoServer<SocketTCP, SocketTCP>  server;
    server.Address("127.0.0.1", 5001);

    int countClient = 250;
    std::vector<SClient<SocketTCP>> clientList;

    for(int i = 0; i < countClient; ++i)
    {
        SClient<SocketTCP> c((std::string("SClient") + std::to_string(i)).c_str());
        c.Address("127.0.0.1", 5001);
        clientList.push_back(std::move(c));
    }

    const char *data[] =
    {
        "XXX",
        "TestDataYYY",
        "TestData_12345",
        "TestData_67890",
        "TestData_0987654321",
        "TestData_abcdefghijklmnopqrstuvwxyz"
    };
    const int countData = sizeof(data)/sizeof(*data);

    ClientServerFrameMClient(server, clientList, [&](auto &clientList)
        {
            for (auto &client : clientList)
            {
                for(auto &item : data)
                {
                    client.SendPacket(item);
                    msleep(10);
                }
            }
        }
    );



    for(int i = 0; i < countClient; ++i)
    {
        EXPECT_EQ (countData, clientList[i]._packetList.size());
        if (countData == clientList[i]._packetList.size())

        for(int j = 0; j < countData; ++j)
        {
            StreamPacket::byte_t buff[128];
            int l = clientList[i]._packetList[j].Payload(buff, 128);
            buff[l] = '\0';
            EXPECT_EQ (0, std::strcmp((char*)buff, data[j]));
        }
    }
    server.Release();
}


std::mutex              g_mtx;
std::condition_variable g_cv;
bool                    g_ready = false;

auto thFunctor = [](std::array<StreamPacket, 6> &testPackets) -> void
{
    SocketResult res;

    std::stringstream ss;
    ss << "SClient" << std::this_thread::get_id();
    SClient<SocketTCP> client(ss.str().c_str());
    client.Address("127.0.0.1", 5001);

    EXPECT_EQ(SocketResult::SR_SUCCESS, res = client.Init());
    if (SocketResult::SR_SUCCESS != res) return;

    EXPECT_EQ(SocketResult::SR_SUCCESS, res = client.ConnectServer());
    if (SocketResult::SR_SUCCESS != res) return;

    msleep(1);
    std::thread thRead = std::thread( [&client](){ client.LoopRead(); });

    std::unique_lock<std::mutex> lk(g_mtx);
    g_cv.wait(lk, []{return g_ready;});
    lk.unlock();

    for(const auto &pck : testPackets)
        client.SendPacket(pck);
    for(int i = 0; (i < 30) && client._packetList.size() != testPackets.size(); ++i)
        msleep(5);

    EXPECT_EQ (client._packetList.size(), testPackets.size() );
    if (client._packetList.size() == testPackets.size())
    {
        for(int i = 0; i < testPackets.size(); ++i)
        {
            EXPECT_TRUE(client._packetList[i] == testPackets[i]);
        }
    }

    client.LoopReadStop();
    if (thRead.joinable()) thRead.join();
};



TEST(MemStreamServerTCP, IndividualConcurrent)
{
    LogLineGlbSocketName(g_logSock);

    PacketEchoServer<SocketTCP, SocketTCP>  server;
    server.Address("127.0.0.1", 5001);
    ASSERT_EQ(SocketResult::SR_SUCCESS, server.Init());
    std::thread thServer(   [&server](){server.LoopListen();}   );

    std::array<StreamPacket, 6> testPackets
    {
        "TestDataXXX",
        "TestDataYYY",
        "TestData_12345",
        "TestData_67890",
        "TestData_0987654321",
        "TestData_abcdefghijklmnopqrstuvwxyz"
    };

    int countClient = 100;
    std::thread thClients[countClient];

    for(int i = 0; i < countClient; ++i)
        thClients[i] = std::thread(thFunctor, std::ref(testPackets));
    msleep(500);

    {
        std::unique_lock<std::mutex> lk(g_mtx);
        g_ready = true;
        g_cv.notify_all();
        lk.unlock();
    }

    for (auto &thClient : thClients)
        if(thClient.joinable()) thClient.join();

    server.LoopListenStop();
    if (thServer.joinable()) thServer.join();

    server.Release();
}

