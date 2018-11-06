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


#include "SocketClientPacket.h"
#include "SocketTCP.h"
#include "GeneralUtils.h"
#include "StreamPacket.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <thread>


class SClient : public SocketClientPacket<SocketTCP>
{
    public:
        SClient(const char *name): SocketClientPacket<SocketTCP>(name) {}
//        virtual void OnRecv        (MemStream<std::uint8_t> &&);
        virtual void OnErrorClient (SocketResult);
        virtual void OnRecvPacket  (StreamPacket &&packet);
};

inline void SClient::OnErrorClient (SocketResult err)
{
    std::cout << "ERROR : SClient::OnErrorClient - " << SocketResultText(err) << std::endl;
}

inline void SClient::OnRecvPacket  (StreamPacket &&packet)
{
    msize_t         pyLen = packet.PayloadLen();
    const msize_t   buffLen = 128;
    byte_t          buff [buffLen];

    for (msize_t i = 0; i < pyLen; i += buffLen)
    {
        if (packet.PayloadPart(buff, buffLen, i) > 0)
        {
            LOG_LINE_GLOBAL("ClientEcho", "Packet:", std::string((char*)buff, pyLen));
            std::cout << "Packet Reveived : " << std::string((char*)buff, pyLen) << std::endl;
        }
    }
}










bool send(SocketTCP &sock, const void *data, std::uint32_t len)
{
    ssize_t res = (len > 0) ? sock.Write(data, len) : 0;
    return ((res > 0) && (res == (ssize_t)len));
}


bool send(SocketTCP &sock, const StreamPacket &pack)
{
    const StreamPacket::byte_t *b;
    std::uint32_t l = pack.Buffer(&b);

    ssize_t res = (l > 0) ? sock.Write(b, l) : 0;
    return ((res > 0) && (res == (ssize_t)l));
}

bool send_StreamPacket(SocketTCP &sock, const char *data)
{
    std::cout << "Sending : " << data << std::endl;
    return send(sock, std::move(StreamPacket(data, strlen(data))));
}


void SendMultipleIndividual(SocketTCP &sock)
{
    char  buff[32] = "MulIndividual-";
    for (int i = 0; i < 100; ++i)
    {
        strcpy(buff+11, std::to_string(i).c_str());
        send(sock, std::move(StreamPacket(buff, std::strlen(buff))));
    }
}


void SendBulkIndividual(SocketTCP &sock)
{
    std::string           prefix = "BulkIndividual-";
    StreamPacket::byte_t  buff2[4096];
    StreamPacket::byte_t *pBuff = nullptr;
    int  len    = 0;
    int  lenPck = 0;

    for (int i = 0; i < 100; ++i)
    {
        std::string data = prefix +  std::to_string(i);
        StreamPacket packet(data.c_str(), data.size());

        lenPck = packet.Buffer(&pBuff);
        std::memcpy(buff2 + len, pBuff, lenPck);
        len += lenPck;
    }

    std::cout << "sending  len = " << len << std::endl;
    send(sock, buff2, len);
}



void SendDirt(SocketTCP &sock)
{
    std::string           prefix = std::string("Dirt-")+std::to_string(getpid());
    StreamPacket::byte_t  buff[4096];
    StreamPacket::byte_t *pBuff  = nullptr;
    int                   len    = 0;
    int                   lenPck = 0;

    std::string data = prefix + "Individual";
    StreamPacket packet(data.c_str(), data.size());

    lenPck = packet.Buffer(&pBuff);

    std::memcpy(buff + len, "XXX", 3);
    len+=3;

    std::memcpy(buff + len, pBuff, lenPck);
    len += lenPck;

    std::memcpy(buff + len, "YYY", 3);
    len+=3;

    std::cout << "sending  len = " << len << std::endl;
    send(sock, buff, len);
}

void SendDirtBulk(SocketTCP &sock)
{
    std::string           prefix = "Dirt-";
    StreamPacket::byte_t  buff[4096];
    StreamPacket::byte_t *pBuff = nullptr;
    int  len  = 0;
    int  lenPck = 0;
    for (int i = 0; i < 40; ++i)
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
    send(sock, buff, len);
}


void SendCorrupt(SocketTCP &sock)
{
    std::string           prefix = "Corrupt-Individual";
    StreamPacket::byte_t  buff2[4096];
    StreamPacket::byte_t *pBuff  = nullptr;
    int                   len    = 0;
    int                   lenPck = 0;

    std::string data = prefix;
    StreamPacket packet(data.c_str(), data.size());

    lenPck = packet.Buffer(&pBuff);
    pBuff[13] = 'X';
    std::memcpy(buff2 + len, pBuff, lenPck);
    len += lenPck;

    std::cout << "sending  len = " << len << std::endl;
    send(sock, buff2, len);
}


void SendCorruptBulk(SocketTCP &sock)
{
    std::string           prefix = "Corrupt-";
    StreamPacket::byte_t  buff2[4096];
    StreamPacket::byte_t *pBuff = nullptr;
    int  len    = 0;
    int  lenPck = 0;

    for (int i = 0; i < 1; ++i)
    {
        std::string data = prefix +  std::to_string(i);
        StreamPacket packet(data.c_str(), data.size());

        lenPck = packet.Buffer(&pBuff);
        //pBuff[16] = 'X';
        pBuff[13] = 'X';
        std::memcpy(buff2 + len, pBuff, lenPck);
        len += lenPck;
    }

    std::cout << "sending  len = " << len << std::endl;
    send(sock, buff2, len);
}




int main(int , char** )
{
    const char   *slog  = "/home/postgres/.sock_domain_log";
    const char   *sname = "127.0.0.1";
    std::uint16_t sport = 5001;

    std::cout << "StreamServerClient v0.0.0.2\n";
    std::cout << "Using Log Server : " << slog << std::endl;
    std::cout << "Using Server     : " << sname << ":" << sport << std::endl;


    LogLineGlbSocketName(slog);
    LOG_LINE_GLOBAL("ClientEcho", "ver 0.0.0.0");


    SClient sock("Sender");
    sock.Address(sname, sport);

    if (SocketResult::SR_SUCCESS != sock.Init())
    {
        std::cerr << "ERROR : Unable to init SocketTCP \n";
        return -1;
    }

    if (SocketResult::SR_SUCCESS != sock.ConnectServer())
    {
        std::cerr << "ERROR : Unable to Connect SocketTCP \n";
        return -2;
    }
    std::thread th ( [&sock] () { sock.LoopStart();} );
    sleep(1);


/*
    {
        const char *data = nullptr;
        //Invidiual


        send_StreamPacket(sock, "xxx");
        send_StreamPacket(sock, "TestData_12345");
        send_StreamPacket(sock, "TestData_67890");
        sleep(5);
        send_StreamPacket(sock, "TestData_abcde");
        send_StreamPacket(sock, "TestData_fghiy");
    }
*/

/*
    {
        //Combined Individual
        StreamPacket pck1("TestDataCombinedPacket_1_1x345", 30);
        StreamPacket pck2("TestDataCombinedPacket_2_6x890", 30);

        StreamPacket::byte_t *bufferPrt1;
        StreamPacket::byte_t *bufferPrt2;

        int  lenBuffer1 = pck1.Buffer(&bufferPrt1);
        int  lenBuffer2 = pck2.Buffer(&bufferPrt2);

        StreamPacket::byte_t data[lenBuffer1 + lenBuffer2];
        std::memcpy(data,            bufferPrt1, lenBuffer1);
        std::memcpy(data+lenBuffer1, bufferPrt2, lenBuffer2);

        send(sock, data, lenBuffer1 + lenBuffer2);
    }
*/

/*
    {
        //Fragmanted
        StreamPacket pck("TestDataFragmanted67890", 23);

        StreamPacket::byte_t *bufferPrt;
        int  lenBuffer = pck.Buffer(&bufferPrt);

        int  lenPart1 = lenBuffer/2;
        StreamPacket::byte_t part1[lenPart1];
        std::memcpy(part1, bufferPrt, lenPart1);
        send(sock, part1, lenPart1);
        sleep(2);

        int     lenPart2 = lenBuffer - lenPart1;
        StreamPacket::byte_t  part2[lenPart2];
        std::memcpy(part2, bufferPrt+lenPart1, lenPart2);
        send(sock, part2, lenPart2);
    }
*/

/*
    {
        //Dirt
        StreamPacket pck("TestDataDirt:abcdefg", 20);

        StreamPacket::byte_t *bufferPrt;
        int lenBuffer = pck.Buffer(&bufferPrt);

        StreamPacket::byte_t data[lenBuffer + 10];
        data[0] = 'A';
        data[1] = 'B';
        data[2] = 'C';
        data[3] = 'D';
        std::memcpy(data+4, bufferPrt, lenBuffer);
        data[4 + lenBuffer + 1] = 'E';
        data[4 + lenBuffer + 2] = 'F';
        data[4 + lenBuffer + 3] = 'G';
        data[4 + lenBuffer + 4] = 'H';

        send(sock, data, lenBuffer+8);
    }
*/

/*
    {
        //Corrupt
        StreamPacket pck("hijklmnopqrstuvwxyz", 19);

        StreamPacket::byte_t *bufferPrt;
        int lenBuffer = pck.Buffer(&bufferPrt);
        bufferPrt[16] = '.';

        std::cout << "Sending : Corrupted Packet\n";
        send(sock, pck);

        send_StreamPacket(sock, "yyy");
    }
*/

/*
    for(int i = 0; i < 50; ++i)
    {
        SendMultipleIndividual(sock);
        SendBulkIndividual(sock);

        SendBulkDirt(sock);
        SendBulkCorrupt(sock);

        send(sock, std::move(StreamPacket("12345", 5)));
    }
*/


    {
        send_StreamPacket(sock, (std::string("aaa.") + std::to_string(getpid())).c_str() );
        SendDirt(sock);
        send_StreamPacket(sock, (std::string("bbb.") + std::to_string(getpid())).c_str() );
        SendCorrupt(sock);
        send_StreamPacket(sock, (std::string("ccc.") + std::to_string(getpid())).c_str() );
    }



    std::cout << "Sleeping before exit\n";
    sleep(10);
    //std::cout << "<\n";
    sock.LoopStop();
    if (th.joinable())  th.join();

    return 0;
}

