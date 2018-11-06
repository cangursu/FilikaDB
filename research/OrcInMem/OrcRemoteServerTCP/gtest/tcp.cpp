/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "main.h"


#include <mutex>
#include <condition_variable>
#include <list>
#include <stdio.h>


int g_waitAfterSend = 3;


TEST(OrcRemoteServer, Simple)
{
    LogLineGlbSocketName(g_logSock);

    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        //const char *data = "Medyasoft Danışmanlık ve Eğitim A.Ş.";
        const char *data = "12345";
        const MemStream<StreamPacket::byte_t> stream(data, std::strlen(data));

        std::string refidWrite = client.SendPacketWrite("Source-1", data);
        EXPECT_FALSE(refidWrite.empty());
        msleep(g_waitAfterSend);

        bool bFoundRefID = false;
        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidWrite)
            {
                bFoundRefID = true;
                EXPECT_EQ(cmd._result, std::string("SUCCESS") );

                EXPECT_EQ(std::strtol(cmd._buffer._length.c_str(), nullptr, 0), cmd._buffer._buf.size());
            }
        }
        EXPECT_TRUE(bFoundRefID);


        std::string refidRead = client.SendPacketRead ("Source-1", std::strlen(data), 0);
        EXPECT_FALSE(refidRead.empty());
        msleep(g_waitAfterSend);


        bFoundRefID = false;
        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidRead)
            {
                bFoundRefID = true;
                EXPECT_EQ(stream, cmd.DecodeBuffer());
                EXPECT_EQ(std::strtol(cmd._buffer._length.c_str(), nullptr, 10), cmd._buffer._buf.size());
            }
            else
            {
                EXPECT_NE(stream, cmd.DecodeBuffer());
            }
        }
        EXPECT_TRUE(bFoundRefID);

    });
}


TEST(OrcRemoteServer, ReadNoneExist)
{
    LogLineGlbSocketName(g_logSock);
    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        std::string refidRead = client.SendPacketRead ("Source-XXX", 10, 0);
        EXPECT_FALSE(refidRead.empty());

        msleep(g_waitAfterSend);

        bool bFoundRefID = false;
        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidRead)
            {
                EXPECT_EQ(cmd._result, "ERROR");
                EXPECT_EQ(std::strtol(cmd._buffer._length.c_str(), nullptr, 10), cmd._buffer._buf.size());
                bFoundRefID = true;
            }
        }
        EXPECT_TRUE(bFoundRefID);
    });
}


TEST(OrcRemoteServer, Delete)
{
    LogLineGlbSocketName(g_logSock);
    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        const char *data = "Medyasoft";
        MemStream<StreamPacket::byte_t> stream;
        stream.Write(data, std::strlen(data));

        std::string refidWrite = client.SendPacketWrite("Source-1", data);
        EXPECT_FALSE(refidWrite.empty());

        std::string refidRead = client.SendPacketRead("Source-1", std::strlen(data), 0);
        EXPECT_FALSE(refidRead.empty());

        msleep(g_waitAfterSend);

        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidWrite)
            {
                EXPECT_STREQ(cmd._result.c_str(), "SUCCESS");
            }
            else if(cmd._refid == refidRead)
            {
                EXPECT_STREQ(cmd._result.c_str(), "SUCCESS");
                EXPECT_EQ(stream, cmd.DecodeBuffer());
                EXPECT_EQ(std::strtol(cmd._buffer._length.c_str(), nullptr, 10), cmd._buffer._buf.size());
            }
        }


        std::string refidDelete = client.SendPacketDelete("Source-1");
        EXPECT_FALSE(refidDelete.empty());
        msleep(g_waitAfterSend);

        bool bFound = false;
        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidDelete)
            {
                bFound = true;
                EXPECT_EQ(cmd._result, std::string("SUCCESS") );
            }
        }
        EXPECT_TRUE (bFound);


        std::string refidRead2 = client.SendPacketRead("Source-1", std::strlen(data), 0);
        EXPECT_FALSE(refidRead2.empty());
        msleep(g_waitAfterSend);

        bFound = false;
        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidRead2)
            {
                bFound = true;
                EXPECT_EQ(cmd._result, std::string("ERROR") );
            }
        }
        EXPECT_TRUE (bFound);


    });
}


TEST(OrcRemoteServer, DeleteNoneExist)
{
    LogLineGlbSocketName(g_logSock);
    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        std::string refidDelete = client.SendPacketDelete("Source-XXX");
        EXPECT_FALSE(refidDelete.empty());
        msleep(g_waitAfterSend);

        bool bFound = false;
        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidDelete)
            {
                bFound = true;
                ASSERT_STREQ(cmd._result.c_str(), "ERROR");
            }
        }
        EXPECT_TRUE (bFound);
    });
}


TEST(OrcRemoteServer, SimpleSendPartial)
{
    LogLineGlbSocketName(g_logSock);
    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        const char* dataArr[]
        {
            "12345",
            "67890",
            "abcdefghi",
            "jkl",
            "mnopqrstuvwxyz",
            "ABC",
            "DEFGHIJKLMNO",
            "PQRSTUVWXY",
            "Z",
        };


        for(const auto data : dataArr)
        {
            MemStream<StreamPacket::byte_t> stream;
            stream.Write(data, std::strlen(data));

            std::string refidWrite = client.SendPacketWrite("Source-7", data);
            EXPECT_FALSE(refidWrite.empty());
            msleep(g_waitAfterSend);

            bool bFoundRefID = false;
            for (const auto cmd : client._receivedCmds)
            {
                if(cmd._refid == refidWrite)
                {
                    bFoundRefID = true;
                    EXPECT_EQ(cmd._result, std::string("SUCCESS") );
                }
            }
            EXPECT_TRUE(bFoundRefID);
        }


        size_t lenExpected = 0;
        MemStream<StreamPacket::byte_t> streamExpected;
        for (const auto data : dataArr)
        {
            size_t len = std::strlen(data);
            lenExpected += len;
            streamExpected.Write(data, len);
        }

        std::string refidRead = client.SendPacketRead ("Source-7", lenExpected, 0);
        EXPECT_FALSE(refidRead.empty());
        msleep(g_waitAfterSend*3);

        bool bFoundRefID = false;
        for (const auto cmd : client._receivedCmds)
        {

            if(cmd._refid == refidRead)
            {
                bFoundRefID = true;
                EXPECT_EQ(streamExpected, cmd.DecodeBuffer());
                EXPECT_EQ(std::strtol(cmd._buffer._length.c_str(), nullptr, 10), cmd._buffer._buf.size());
            }
            else
            {
                EXPECT_NE(streamExpected, cmd.DecodeBuffer());
            }
        }
        EXPECT_TRUE(bFoundRefID);
    });
}


TEST(OrcRemoteServer, SimpleReadPartial)
{
    LogLineGlbSocketName(g_logSock);
    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        const char* data =  "Atatürk'ün Gençliğe Hitabesi\n"
                            "\n"
                            "Ey Türk Gençliği!\n"
                            "\n"
                            "Birinci vazifen, Türk istiklâlini, Türk Cumhuriyetini, ilelebet, muhafaza ve müdafaa etmektir.\n"
                            "\n"
                            "Mevcudiyetinin ve istikbalinin yegâne temeli budur. Bu temel, senin, en kıymetli hazinendir. İstikbalde dahi, seni bu hazineden mahrum etmek isteyecek, dahilî ve haricî bedhahların olacaktır. Bir gün, İstiklâl ve Cumhuriyeti müdafaa mecburiyetine düşersen, vazifeye atılmak için, içinde bulunacağın vaziyetin imkân ve şerâitini düşünmeyeceksin! Bu imkân ve şerâit, çok nâmüsait bir mahiyette tezahür edebilir. İstiklâl ve Cumhuriyetine kastedecek düşmanlar, bütün dünyada emsali görülmemiş bir galibiyetin mümessili olabilirler. Cebren ve hile ile aziz vatanın, bütün kaleleri zaptedilmiş, bütün tersanelerine girilmiş, bütün orduları dağıtılmış ve memleketin her köşesi bilfiil işgal edilmiş olabilir. Bütün bu şerâitten daha elîm ve daha vahim olmak üzere, memleketin dahilinde, iktidara sahip olanlar gaflet ve dalâlet ve hattâ hıyanet içinde bulunabilirler. Hattâ bu iktidar sahipleri şahsî menfaatlerini, müstevlilerin siyasi emelleriyle tevhit edebilirler. Millet, fakr ü zaruret içinde harap ve bîtap düşmüş olabilir.\n"
                            "\n"
                            "Ey Türk istikbalinin evlâdı! İşte, bu ahval ve şerâit içinde dahi, vazifen; Türk İstiklâl ve Cumhuriyetini kurtarmaktır! Muhtaç olduğun kudret, damarlarındaki asil kanda mevcuttur!\n"
                            "\n"
                            "Mustafa Kemal Atatürk\n"
                            "20 Ekim 1927\n"
                            "\n";

        const size_t lenPartWrite = 15;
        const size_t lenPartRead  = 19;
        const size_t len          = std::strlen(data);

        for(size_t i = 0; i < len; i += lenPartWrite)
        {
            std::string refidWrite = client.SendPacketWrite("Source-7", data + i, std::min(lenPartWrite, (len - i)));
            EXPECT_FALSE(refidWrite.empty());
            msleep(g_waitAfterSend + 7);

            bool bFoundRefID = false;
            for (const auto cmd : client._receivedCmds)
            {
                if(cmd._refid == refidWrite)
                {
                    bFoundRefID = true;
                    EXPECT_EQ(cmd._result, std::string("SUCCESS") );
                }
            }
            EXPECT_TRUE(bFoundRefID);
        }


        for(size_t i = 0; i < len; i += lenPartRead)
        {
            std::string refidRead = client.SendPacketRead ("Source-7", lenPartRead, i);
            EXPECT_FALSE(refidRead.empty());
            msleep(g_waitAfterSend);

            bool bFoundRefID = false;
            for (const auto cmd : client._receivedCmds)
            {
                MemStream<StreamPacket::byte_t> bufferRecieved = cmd.DecodeBuffer();
                MemStream<StreamPacket::byte_t> bufferExpected;
                bufferExpected.Write(data + i, std::min(lenPartRead, (len - i)));
                //msleep(1);

                if(cmd._refid == refidRead)
                {
                    bFoundRefID = true;
                    EXPECT_EQ(bufferExpected, bufferRecieved);

                    char b[bufferRecieved.Len()+1] = "";
                    bufferRecieved.Read(b, bufferRecieved.Len());
                    std::cout << b ;
                }
                else
                {
                    EXPECT_NE(bufferExpected, bufferRecieved);
                }
            }
            EXPECT_TRUE(bFoundRefID);
        }
    });
}


TEST(OrcRemoteServer, Length)
{
    LogLineGlbSocketName(g_logSock);
    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        std::string refidWrite = client.SendPacketWrite("Source-Length", "1234567890");
        EXPECT_FALSE(refidWrite.empty());

        std::string refidLength = client.SendPacketLength("Source-Length");
        EXPECT_FALSE(refidLength.empty());
        msleep(g_waitAfterSend);

        bool bFound = false;
        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidLength)
            {
                bFound = true;
                EXPECT_EQ(cmd._result, std::string("SUCCESS") );
                EXPECT_EQ(cmd._buffer._length, std::string("10") );
            }
        }
        EXPECT_TRUE (bFound);
    });
}


TEST(OrcRemoteServer, LengthNoneExist)
{
    LogLineGlbSocketName(g_logSock);
    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        std::string refidDelete = client.SendPacketLength("Source-XXX");
        EXPECT_FALSE(refidDelete.empty());
        msleep(g_waitAfterSend);

        bool bFound = false;
        for (const auto cmd : client._receivedCmds)
        {
            if(cmd._refid == refidDelete)
            {
                bFound = true;
                EXPECT_EQ(cmd._result, std::string("ERROR") );
            }
        }
        EXPECT_TRUE (bFound);
    });
}







size_t FileContentLength(auto &client, const char *fname)
{
    size_t len = (size_t)-1;
    std::string refid = client.SendPacketLength(fname);
    EXPECT_FALSE(refid.empty());
    msleep(g_waitAfterSend+10);

    for (const auto &cmd : client._receivedCmds)
    {
        if (cmd._refid == refid)
        {
            EXPECT_STREQ(cmd._result.c_str(), "SUCCESS");
            len = std::strtol(cmd._buffer._length.c_str(), nullptr, 10);
            break;
        }
    }
    EXPECT_NE((size_t)-1, len);

    return len;
}

void FileContentSend(auto &client, const char *source, const char *fname)
{
    const size_t blockSizeSend  = _1K;
    size_t       blockCountSend = 0;

    StreamPacket::byte_t blockSend[blockSizeSend];

    FILE *fp = fopen(fname, "rb");
    EXPECT_FALSE(nullptr == fp);

    if (fp)
    {
        size_t len = FileSize(fname);

        std::vector<std::string> refIDsSend(blockCountSend = int(double(len)/double(blockSizeSend))+1);

        for (int idx = 0; !feof(fp); idx++)
        {
            size_t r = fread (blockSend, 1, blockSizeSend, fp);

            std::string refid = client.SendPacketWrite(source, blockSend, r);
            EXPECT_FALSE(refid.empty());
            refIDsSend[idx] = refid;
            //msleep(1);
        }
        sleep(10);

        std::string success = "SUCCESS";
        for (const auto &rid : refIDsSend)
        {
            bool found = false;
            for (const auto &cmd : client._receivedCmds)
            {
                if (cmd._refid == rid)
                {
                    found = true;
                    EXPECT_EQ(cmd._result, success);
                     break;
                }
            }
            EXPECT_TRUE(found);
        }
        fclose(fp);
    }
}


void FileContentRead(auto &client, const char *source, const char *fname)
{
    const size_t blockSizeRead  = _1K;
    size_t len                  = FileContentLength(client, source);
    size_t offset               = 0;
    size_t blockCountRead       = int(double(len)/double(blockSizeRead));

    StreamPacket::byte_t blockRead[blockSizeRead];

    std::vector<std::string> refIDsRead;
    while (offset < len)
    {
        std::string refid = client.SendPacketRead(source, blockSizeRead, offset);
        EXPECT_FALSE(refid.empty());
        if (refid.empty())
        {
            std::cout << "offset : " << offset;
            return;
        }

        //std::cout << "Read - refid:" << refid << " - offset:" << offset <<  " len:" << len << " blockSizeRead:" << blockSizeRead << std::endl;
        refIDsRead.push_back(refid);
        offset += blockSizeRead;
        msleep(3);
    }


    for (const auto &rid : refIDsRead)
    {
        bool found = false;
        for (const auto &cmd : client._receivedCmds)
        {
            if (cmd._refid == rid)
            {
                found = true;
                EXPECT_EQ(cmd._result, "SUCCESS");
                EXPECT_EQ(std::strtol(cmd._buffer._length.c_str(), nullptr, 10), cmd._buffer._buf.size());
                break;
            }
        }
        EXPECT_TRUE(found);
    }


    FILE *fp = fopen(fname, "wb");
    EXPECT_NE(nullptr, fp);

    if (fp)
    {
        for (const auto &cmd : client._receivedCmds)
        {
            if (cmd._source == source &&
                cmd._cmdid  == MemStreamPacket::CmdID::CMD_READ &&
                cmd._result == "SUCCESS")
            {
                auto buffDecoded = cmd.DecodeBuffer();
                std::uint64_t l = buffDecoded.Len();
                char buff[l];
                std::uint64_t rl = buffDecoded.Read(buff, l, 0);
                fwrite(buff, 1, rl, fp);
            }
        }

        fclose(fp);
    }
}




TEST(OrcRemoteServer, SimpleLargeData)
{
    LogLineGlbSocketName(g_logSock);
    ClientServerFrame<SocketTCP, OrcRemoteServerClient>( [](auto &client) -> void
    {
        const char *fname     = "./OrcRemoteServerTCP/gtest/ImageSource.PNG";
        const char *fnameNew  = "./OrcRemoteServerTCP/gtest/ImageSourceNew.PNG";
        const char *source    = fname;

//        std::remove(fnameNew);

        FileContentSend(client, source, fname);
        FileContentRead(client, source, fnameNew);
        EXPECT_TRUE(CompareFile(fname, fnameNew));

//        std::remove(fnameNew);
    });
}


