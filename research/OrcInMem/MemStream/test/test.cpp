


#include "MemStream.h"
#include "StreamPacket.h"
#include "MemStreamPacket.h"

#include <iostream>


bool CheckBufferWrite(MemStream<std::uint8_t> *m, const char *data)
{
    size_t len = std::strlen(data);
    m->Write(data, len);
    std::cout << "Written : " << data << " - " << "Len:" << m->Len() << ", Size:" << m->Size() << ", Count:" << m->Cnt() << std::endl;

    char buff[len + 1] = "";
    m->Read(buff, len, m->Len() - len);
    std::cout << "Readed : " << std::string(buff, len) << std::endl;

    return std::strncmp(buff, data, len) == 0;
}

void Test_MemStream()
{
    std::cout << "MemStream Test Suit\n";
    int resTest = true;
    int isPass  = true;

    MemStream<std::uint8_t> m;

    const char *data[] =
    {
        "",
        "1234567890abcdefghi",
        "jkl",
        "mn",
        "opqr",
        nullptr
    };

    for (const char **d = data; *d; ++d)
    {
        std::cout << "Adding data : " << *d << std::endl;
        resTest = CheckBufferWrite(&m, *d);
        isPass &= resTest;
    }


    std::cout << "******* Test " << (isPass ? "SUCCEED" : "FAILED") << " *******\n";
}





void Test_StreamPacket()
{
    std::cout << "StreamPacket Test Suit\n";

    StreamPacket          pck;
    StreamPacket::byte_t  buff[g_lenMaxPayload];


    // -----------------
    // -- 1. test
    const char *data = "denemem";
    int         len  = strlen(data) * sizeof(char);

    pck.Create(data, len);
    if (false == pck.Check())
        std::cout << "********* Test_StreamPacket FAILED *********\n";

    pck.Payload(buff, len);
    std::cout << "data = " << data << " - payload = " << std::string((char *)buff, len) << std::endl;
    if (std::memcmp(buff, data, len))
        std::cout << "********* Test_StreamPacket FAILED *********\n";


    // -----------------
    // -- 2. test

    data = "kenemem";
    len  = strlen(data) * sizeof(char);

    pck.Create(data, len);
    if (false == pck.Check()) std::cout << "********* Test_StreamPacket FAILED *********\n";

    pck.Payload(buff, len);
    std::cout << "data = " << data << " - payload = " << std::string((char *)buff, len) << std::endl;
    if (std::memcmp(buff, data, len)) std::cout << "********* Test_StreamPacket FAILED *********\n";

    // -----------------
    // -- 2. test
//    pck._buff[11] = 'X';
//    if (true == pck.Check()) std::cout << "********* Test_StreamPacket FAILED *********\n";

    // -----------------
    // -- 3. test
    char data2[len = g_lenMaxPayload];
    for (int i = 0; i < len;++i) data2[i] = (char)i;

    pck.Create(data2, len);
    if (false == pck.Check()) std::cout << "********* Test_StreamPacket FAILED *********\n";
    pck.Payload(buff, len);
    if (std::memcmp(buff, data2, len)) std::cout << "********* Test_StreamPacket FAILED *********\n";



    std::cout << "StreamPacket Test Suit DONE\n";
}


void Test_MemStreamPacket()
{
    char *buf   = "XXX";
    int  length = 3;

    MemStreamPacket packet;
    int i = packet.CreatePacketWrite("TableXXX", buf, length);

    MemStreamPacket::byte_t payload[1024] = "";
    int len = packet.Payload(payload, 1024);
    std::cout << "payload len  : " << len << std::endl;
    std::cout << "payload data : " << payload << std::endl;
}

int main(int argc, char** argv)
{
    //Test_StreamPacket();
    Test_MemStreamPacket();
}


