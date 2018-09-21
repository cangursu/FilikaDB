

#include "main.h"
#include "StreamPacket.h"

#include <gtest/gtest.h>


const char *g_logSock = "/home/postgres/.sock_domain_log";
const char *g_srvSock = "/home/postgres/.sock_domain_pgext";
const char *g_srvAddr = "127.0.0.1";
int         g_srvPort = 5001;




std::string MemStream2String(MemStream<std::uint8_t> &stream)
{
    std::string     strData;
    std::uint64_t   rsize = 0;
    const int       bsize = 256;
    char            buff[bsize];

    for (std::uint64_t offset = 0, len = stream.Len(); offset < len; offset += bsize)
    {
        memset(buff, 0, bsize);
        rsize = stream.Read(buff, bsize, offset);
        strData += std::string(buff, rsize);
    }

    return std::move(strData);
}


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








int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

