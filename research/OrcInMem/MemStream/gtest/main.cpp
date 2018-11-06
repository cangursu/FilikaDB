

#include "main.h"

#include "MemStream.h"
#include "StreamPacket.h"
#include "MemStreamPacket.h"
#include "TimeUtils.h"
#include "GeneralUtils.h"

#include <gtest/gtest.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"



TEST(MemStream, Test1)
{
    StreamPacket::byte_t buff[g_lenMaxPayload];

    const char *data = "denemem";
    int         len  = strlen(data) * sizeof(char);

    StreamPacket pck;

    ASSERT_EQ  (pck.Create(data, len), g_lenOverhead + len);
    ASSERT_TRUE(pck.Check());

    ASSERT_EQ  (pck.Payload(buff, len)      , len);
    ASSERT_EQ  (std::memcmp(buff, data, len), 0);

    data = "kenemem";
    len  = strlen(data) * sizeof(char);

    ASSERT_EQ  (pck.Create(data, len), g_lenOverhead + len);
    ASSERT_TRUE(pck.Check());

    ASSERT_EQ  (pck.Payload(buff, len)      , len);
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

    ASSERT_EQ  (pck.Payload(buff, len)      , len);
    ASSERT_EQ  (0, std::memcmp(buff, data   , len));
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


TEST(MemStreamPacket, Base64Buffer)
{
    const char *        testData = "0123456789abcdefghijklmnopqrstuvwxyz";
    const std::uint64_t lenData = std::strlen(testData);

    const char *        testData64 = "MDEyMzQ1Njc4OWFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6";
    const std::uint64_t lenData64 = std::strlen(testData64);

    std::uint64_t       bufferLen  = int(lenData * (4.0/3.0) + 3.0);
    char                buffer[bufferLen];

    MemStream<StreamPacket::byte_t> stream;
    stream.Write(testData, lenData);

    EncodeBase64(stream, buffer, bufferLen);
    EXPECT_EQ(bufferLen, lenData64);
    EXPECT_EQ(0, std::memcmp(buffer, testData64, bufferLen));

    stream.reset();
    stream.Write(buffer, bufferLen);

    DecodeBase64(stream, buffer, bufferLen);
    EXPECT_EQ(bufferLen, lenData);
    EXPECT_EQ(0, std::memcmp(buffer, testData, bufferLen));
}


TEST(MemStreamPacket, Base64Stream)
{
    const char *        testData = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0987654321";
    const std::uint64_t lenData = std::strlen(testData);

    const char *        testData64 = "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVowOTg3NjU0MzIx";
    const std::uint64_t lenData64 = std::strlen(testData64);

    MemStream<StreamPacket::byte_t> stream;
    stream.Write(testData, lenData);

    MemStream<StreamPacket::byte_t> stream64;
    stream64.Write(testData64, lenData64);

    MemStream<StreamPacket::byte_t> streamTest1;
    MemStream<StreamPacket::byte_t> streamTest2;

    EncodeBase64(stream, streamTest1);
    EXPECT_EQ(stream64.Len(), streamTest1.Len());
    EXPECT_EQ(stream64, streamTest1);

    DecodeBase64(stream64, streamTest2);
    EXPECT_EQ(stream.Len(), streamTest2.Len());
    EXPECT_EQ(stream, streamTest2);
}


TEST(MemStreamPacket, b64Jsn1)
{
    typedef rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>, rapidjson::MemoryPoolAllocator<>> DocumentType;

    char valueBuffer[_8K];
    char parseBuffer[_2K];
    rapidjson::MemoryPoolAllocator<> valueAllocator(valueBuffer, sizeof(valueBuffer));
    rapidjson::MemoryPoolAllocator<> parseAllocator(parseBuffer, sizeof(parseBuffer));

    //rapidjson::Document doc;
    DocumentType doc(&valueAllocator, sizeof(parseBuffer), &parseAllocator);
    doc.SetObject();

    rapidjson::Document::AllocatorType &a = doc.GetAllocator();
    rapidjson::Value cmdItem;
    cmdItem.SetObject();


    MemStream<StreamPacket::byte_t> data;
    data.Write("0123456789", 10);
    MemStreamPacket::JsonDataB64(cmdItem, a, 10, 0, data);

    rapidjson::Value cmdArray(rapidjson::kArrayType);
    cmdArray.PushBack(cmdItem, a);
    doc.AddMember("MYSFRIF", cmdArray, a);

    rapidjson::StringBuffer sbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sbuf);
    doc.Accept(writer);

    char *check =   "{\n"
                    "    \"MYSFRIF\": [\n"
                    "        {\n"
                    "            \"Buffer\": {\n"
                    "                \"Lenght\": \"16\",\n"
                    "                \"Enc\": \"Base64\",\n"
                    "                \"Content\": \"MDEyMzQ1Njc4OQ==\"\n"
                    "            }\n"
                    "        }\n"
                    "    ]\n"
                    "}";


    EXPECT_STREQ(check, sbuf.GetString());
    msleep(1);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

