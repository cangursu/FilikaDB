


#include "main.h"
#include <gtest/gtest.h>

int nsleep(long nanoseconds)
{
    timespec rem{0,0}, req {.tv_sec = 0, .tv_nsec = nanoseconds};
    nanosleep(&req, &rem);
}

int msleep(long miliseconds)
{
   timespec req, rem;

   if(miliseconds > 999)
   {
        req.tv_sec  = (int)(miliseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (miliseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }
   else
   {
        req.tv_sec  = 0;                         /* Must be Non-Negative */
        req.tv_nsec = miliseconds * 1000000;    /* Must be in range of 0 to 999999999 */
   }

   return nanosleep(&req , &rem);
}

std::string MemStream2String(MemStream<std::uint8_t> &stream)
{
    std::string     strData;
    std::uint64_t   rsize = 0;
    const int       bsize = 256;
    char            buff[bsize];

    for (std::uint64_t offset = 0, len = stream.Len(); offset < len; offset += bsize)
    {
        memset(buff, 0, bsize);
        rsize = stream.read(buff, bsize, offset);
        strData += std::string(buff, rsize);
    }

    return std::move(strData);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

