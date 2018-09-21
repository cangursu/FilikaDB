

#include "main.h"

#include "MemStream.h"
#include "StreamPacket.h"

#include <gtest/gtest.h>



TEST(MemStream, Test1)
{
    StreamPacket::byte_t buff[g_lenMaxPayload];

    const char *data = "denemem";
    int         len  = strlen(data) * sizeof(char);

    StreamPacket pck;

    ASSERT_EQ  (pck.Create(data, len), g_lenOverhead + len);
    ASSERT_TRUE(pck.Check());

    ASSERT_EQ  (pck.Payload(buff, len), len);
    ASSERT_EQ  (std::memcmp(buff, data, len), 0);

    data = "kenemem";
    len  = strlen(data) * sizeof(char);

    ASSERT_EQ  (pck.Create(data, len), g_lenOverhead + len);
    ASSERT_TRUE(pck.Check());

    ASSERT_EQ  (pck.Payload(buff, len), len);
    ASSERT_EQ  (std::memcmp(buff, data, len), 0);
}


TEST(MemStream, Test2)
{
    StreamPacket::byte_t buff[g_lenMaxPayload];
    StreamPacket pck;

    int  len  = g_lenMaxPayload;
    char data[len];

    for (int i = 0; i < len;++i)
        data[i] = (char)i;

    ASSERT_EQ  (pck.Create(data, len), g_lenOverhead + len);
    ASSERT_TRUE(pck.Check());

    ASSERT_EQ  (pck.Payload(buff, len), len);
    ASSERT_EQ  (0, std::memcmp(buff, data, len));
}



TEST(MemStream, Test3)
{
    MemStream<std::uint8_t> m;

    const char *data[] =
    {
        "",
        "099887",
        "1234567890abcdefghi",
        "76",
        "65",
        "jklmnopqr",
        "1a2b3c4d5e6f7g8h9i0",
        "54",
        "4332",
        "21"
    };

    size_t len    = 0;
    size_t slen   = 0;
    size_t offset = 0;

    for (const auto &part : data)
    {
        slen += (len = std::strlen(part));
        m.Write(part, len);

        ASSERT_EQ(slen, m.Len());
    }


    for (const auto &part : data)
    {
        len = std::strlen(part);

        char buff[len + 1] = "";
        m.Read(buff, len, offset);
        offset += len;

        ASSERT_EQ(0, std::memcmp(buff, part, len));
    }
}


TEST(StreamPacket, Test1)
{
    char *pdata = "DummyTestData";
    int   len   = std::strlen(pdata);

    StreamPacket pck(pdata, len);

    ASSERT_TRUE(pck.Check());
    ASSERT_EQ(len, pck.PayloadLen());

    StreamPacket::byte_t buff[len+1];
    pck.Payload(buff, len);
    ASSERT_EQ(0, std::memcmp(buff, pdata, len));
}


TEST(StreamPacket, Test2)
{
    char *pdata = "DummyTestData";
    int   len   = std::strlen(pdata);

    StreamPacket pck1(pdata, len);
    StreamPacket pck2(pdata, len);

    ASSERT_TRUE(pck1 == pck2);
}




int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

