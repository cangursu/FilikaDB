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


void PrintPacket(const StreamPacket &packet)
{
    StreamPacket::byte_t buff[256];
    int readed = 0;
    for( int offset = 0;  (readed = packet.PayloadPart(buff, 256, offset)) == 256; offset += readed)
        std::cout << (char*)buff;
    if (readed > 0)
        std::cout << std::setw(readed) << std::string((char*)buff, readed);
    std::cout << std::endl;

}



class SClient : public SocketClientPacket<SocketDomain>
{
    public:
        SClient(const char *name): SocketClientPacket<SocketDomain>(name) {}

        virtual void        OnErrorClient (SocketResult);
        virtual void        OnRecvPacket  (StreamPacket &&packet);

        bool                SendPacket(const StreamPacket &pack);
        const StreamPacket &Packet(int idxPacket);

        std::deque <StreamPacket> _packetList;
};

bool SClient::SendPacket(const StreamPacket &pack)
{
    const StreamPacket::byte_t *b;
    std::uint32_t l = pack.Buffer(&b);

    ssize_t res = (l > 0) ? this->Write(b, l) : 0;
    return ((res > 0) && (res == (ssize_t)l));
}
inline void SClient::OnErrorClient (SocketResult err)
{
    std::cout << ".ERROR : SClient::OnErrorClient - " << SocketResultText(err) << std::endl;
}
inline void SClient::OnRecvPacket  (StreamPacket &&packet)
{
    //std::cout << "SClient::OnRecvPacket          - ";
    //PrintPacket(packet);

    _packetList.push_back(std::move(packet));
}
const StreamPacket &SClient::Packet(int idxPacket)
{
    return _packetList[idxPacket];
}




class PacketEchoClient  : public  SocketClientPacket<SocketDomain>
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
            //std::cout << "PacketEchoClient::OnRecvPacket - ";
            //PrintPacket(packet);

            SocketResult res = SendPacket(packet);  //Send Back
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


class PacketEchoServer : public SocketServer<SocketDomain, PacketEchoClient>
{
    public:
        PacketEchoServer()
            : SocketServer<SocketDomain, PacketEchoClient>("ServerEcho")
        {
        }
        virtual void OnAccept(const PacketEchoClient &sock, const sockaddr &addr)
        {
            std::string host, serv;
            if (true == NameInfo(addr, host, serv))
            {
                LOG_LINE_GLOBAL("ServerEcho", "Accepted connection on host = ", host, " port = ", serv);
                ClientCountMsg();
            }
        }

        virtual void OnDisconnect  (const PacketEchoClient &sock)
        {
            LOG_LINE_GLOBAL("ServerEcho", "Client Disconnected.");
            ClientCountMsg();
        }

        virtual void OnErrorClient(const PacketEchoClient &, SocketResult res)
        {
            LOG_LINE_GLOBAL("ServerEcho", "ErrorClient : ", SocketResultText(res));
        }

        virtual void OnErrorServer(SocketResult res)
        {
            LOG_LINE_GLOBAL("ServerEcho", "ErrorServer : ", SocketResultText(res));
        };

        void ClientCountMsg()
        {
            LOG_LINE_GLOBAL("ServerEcho", "Client count = ", SocketServer<SocketDomain, PacketEchoClient>::ClientCount());
        }
};










template <typename TServer, typename TClient, typename TFunctor>
void DomainSockClientServerFrame(TServer &server, TClient &client, TFunctor f)
{
    LogLineGlbSocketName(g_slog);

    //Prepare Server
    //PacketEchoServer server;
    server.SocketPath(g_ssrv);

    ASSERT_EQ(SocketResult::SR_SUCCESS, server.Init());
    std::thread thSrv(   [&server](){server.LoopListen();}   );

    //Prepare Client
    //SClient client("Sender");
    EXPECT_EQ(SocketResult::SR_SUCCESS, client.Init(g_ssrv));
    EXPECT_EQ(SocketResult::SR_SUCCESS, client.Connect());
    msleep(1);
    EXPECT_EQ(1, server.ClientCount());

    std::thread thCln ( [&client] () { client.LoopRead();} );
    msleep(1);

    f(client);

    client.LoopReadStop();
    server.LoopListenStop();

    if (thCln.joinable()) thCln.join();
    if (thSrv.joinable()) thSrv.join();

}



TEST(MemStreamServer, DomainIndividual)
{
    PacketEchoServer server;
    SClient client("Sender");


    std::string dataPrefix("TestDataIndividual");

    int count = 300;
    std::vector<StreamPacket> data(count);

    for (int i = 0; i < count; ++i)
    {
        std::string  payload = dataPrefix + ":" + std::to_string(i);
        data[i] = StreamPacket(payload.c_str(), payload.size());
    }


    DomainSockClientServerFrame(server, client, [&data] (auto &client)
        {
            for (const auto &item : data)
            {
                EXPECT_TRUE(client.SendPacket(item));
                //msleep(1);
            }
            msleep(50);
        }
    );


    const PacketEchoClient *cln = server.ClientByIdx(0);
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


TEST(MemStreamServer, DomainBulkIndividual)
{
    PacketEchoServer server;
    SClient client("Sender");

    int         count   = 5;
    std::string prefix = "BulkIndividual-";

    DomainSockClientServerFrame(server, client, [&count, &prefix] (auto &client)
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


    const PacketEchoClient *cln = server.ClientByIdx(0);
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


TEST(MemStreamServer, DomainDirt)
{
    PacketEchoServer server;
    SClient client("Sender");

    const char *pData = "TestDataDirt:abcdefghijklmnopqrstuvwxyz";

    DomainSockClientServerFrame(server, client, [&pData] (auto &client)
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


    const PacketEchoClient *cln = server.ClientByIdx(0);
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


TEST(MemStreamServer, DomainCorrupt)
{
    int         lenBuffer   = 0;
    const char *pload       = "abcdefghijklmnopqrstuvwxyz";

    PacketEchoServer server;
    SClient client("Sender");

    DomainSockClientServerFrame(server, client, [&lenBuffer, &pload](auto &client)
        {
            StreamPacket pck(pload);

            StreamPacket::byte_t *pbuff;
            lenBuffer = pck.Buffer(&pbuff);

            pbuff[14] = '.';  //Corrupting buffer...

            EXPECT_TRUE(client.SendPacket(pck));
        }
    );



    const PacketEchoClient *cln = server.ClientByIdx(0);
    EXPECT_TRUE(cln != nullptr);
    EXPECT_EQ(0, cln->_packetList.size());
    EXPECT_EQ(lenBuffer - 1, cln->_countError);
}


TEST(MemStreamServer, DomainBulkCorrupt)
{
    int count = 1;

    PacketEchoServer server;
    SClient client("Sender");

    DomainSockClientServerFrame(server, client, [&count](auto &client)
        {
            std::string           prefix = "Corrupt-";
            StreamPacket::byte_t  buff2[4096];
            StreamPacket::byte_t *pBuff = nullptr;

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
}

