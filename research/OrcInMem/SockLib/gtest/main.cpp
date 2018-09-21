


#include "main.h"
#include <gtest/gtest.h>


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


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

