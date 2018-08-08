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


#include "SocketDomain.h"
#include "StreamPacket.h"

#include <iostream>
#include <cstring>
#include <unistd.h>



bool send(SocketDomain &sock, const void *data, std::uint32_t len)
{
    ssize_t res = (len > 0) ? sock.Write(data, len) : 0;
    return ((res > 0) && (res == (ssize_t)len));
}


bool send(SocketDomain &sock, const StreamPacket &pack)
{
    const StreamPacket::byte_t *b;
    std::uint32_t l = pack.Buffer(&b);

    ssize_t res = (l > 0) ? sock.Write(b, l) : 0;
    return ((res > 0) && (res == (ssize_t)l));
}

bool send_StreamPacket(SocketDomain &sock, const char *data)
{
    return send(sock, std::move(StreamPacket(data, strlen(data))));
}


void SendMultipleIndividual(SocketDomain &sock)
{
    char  buff[32] = "MulIndividual-";
    for (int i = 0; i < 100; ++i)
    {
        strcpy(buff+11, std::to_string(i).c_str());
        send(sock, std::move(StreamPacket(buff, std::strlen(buff))));
    }
}


void SendBulkIndividual(SocketDomain &sock)
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


void SendBulkDirt(SocketDomain &sock)
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


void SendBulkCorrupt(SocketDomain &sock)
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
    char *sname = "/home/postgres/.sock_pgext_domain";

    std::cout << "StreamServerClient v0.0.0.2\n";
    std::cout << "Using domain Socket : " << sname << std::endl;

    SocketDomain sock("Sender");
    if (SocketResult::SR_SUCCESS != sock.Init(sname))
    {
        std::cerr << "ERROR: Unable to init SocketDomain \n";
        return 0;
    }

    if (SocketResult::SR_SUCCESS != sock.Connect())
    {
        std::cerr << "ERROR: Unable to Connect SocketDomain \n";
        return 0;
    }



    {
        const char *data = nullptr;
        //Invidiual

        send_StreamPacket(sock, "TestData");
        send_StreamPacket(sock, "TestData_12345");
        send_StreamPacket(sock, "TestData_67890");
        sleep(5);
        send_StreamPacket(sock, "TestData_abcde");
        send_StreamPacket(sock, "TestData_fghiy");
    }
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
        bufferPrt[12] = '.';
        send(sock, pck);
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
    //SendBulkIndividual(sock);
    return 0;
}

