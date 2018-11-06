

#include "main.h"
#include "StreamPacket.h"

#include <gtest/gtest.h>
#include <sys/stat.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

/*
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
*/

long FileSize(const char *fname)
{
    struct stat  st;
    int rc = stat(fname, &st);
    return rc == 0 ? st.st_size : -1;
}


bool CompareFile(FILE* fp1, FILE* fp2)
{
    if (nullptr == fp1 || nullptr == fp2 )
        return false;

    int  buffLen = 10000;
    char buff1[buffLen];
    char buff2[buffLen];

    do
    {
        size_t r1 = fread(buff1, 1, buffLen, fp1);
        size_t r2 = fread(buff2, 1, buffLen, fp2);

        if (r1 != r2 || std::memcmp(buff1, buff2, r1))
        {
            return false;  // Files are not equal
        }
    } while (!feof(fp1) && !feof(fp2));
    return feof(fp1) && feof(fp2);
}

bool CompareFile(const char *f1, const char *f2)
{
    FILE *fp1 = fopen(f1, "rb");
    FILE *fp2 = fopen(f2, "rb");

    bool res = CompareFile(fp1, fp2);
    if (fp1) fclose(fp1);
    if (fp2) fclose(fp2);

    return res;
}






int main(int argc, char **argv)
{
    LogLineGlbSocketName(g_logSock);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

