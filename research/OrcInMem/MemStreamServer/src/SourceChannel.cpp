/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SourceChannel.cpp
 * Author: can.gursu
 *
 * Created on 19 July 2018, 15:22
 */

#include "SourceChannel.h"
#include "LoggerGlobal.h"
#include "SocketUtils.h"

#include <cstring>
#include <iostream>


SourceChannel::SourceChannel(const char *sourceChannelName)
        : SocketDomain(sourceChannelName)
{
}

/*std::thread&& */void SourceChannel::listenAsyc(const char *sourceChannelName, bool async /*= false*/)
{
    //SourceChannel sss(sourceChannelName);
    std::thread th( (SourceChannel(sourceChannelName)) );
    // ...................................
    if (false == async) th.join();

    //return std::move(th);
}

ParseResult SourceChannel::recvScanMID(byte_t by, msize_t posBuff, msize_t posPacket)
{
    //LOG_LINE_GLOBAL("SServerClient", "---> by:", by, "(", std::hex, by, std::dec, ")", " posBuff:", posBuff, " posPacket:", posPacket);

    ParseResult res = ParseResult::ERROR;

    if ((posPacket >= g_posMID) && posPacket < g_posMID + g_lenMID)
    {
        if (posPacket == 0)
            _posBuffBeginPack = posBuff;
        if (by == StreamPacket::s_mid[posPacket])
            res = (posPacket == (g_posMID + g_lenMID - 1)) ? ParseResult::NEXT : ParseResult::CONTINUE;
    }

    //LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
    return res;

}

ParseResult SourceChannel::recvParseDataLength(byte_t by, msize_t posBuff, msize_t posPacket)
{
    //LOG_LINE_GLOBAL("SServerClient", "---> posBuff:", posBuff, " posPacket:", posPacket);

    ParseResult res = ParseResult::ERROR;
    if ((posPacket >= g_posPLen) && (posPacket < g_posPLen + g_lenPLen))
    {
        _lenghtBuff[_lenghtBuffPos++] = by;
        if (posPacket == g_posPLen + g_lenPLen - 1)
        {
            _lenPayload    = 0L;
            _lenghtBuffPos = 0L;

            memcpy(&_lenPayload, _lenghtBuff, g_lenPLen);
            //LOG_LINE_GLOBAL("SServerClient", "_lenPayload:", _lenPayload);
            res = (_lenPayload > g_lenMaxBuffer) ? ParseResult::ERROR : ParseResult::NEXT;
        }
        else
        {
            res = ParseResult::CONTINUE;
        }
    }
    else
    {
        _lenghtBuffPos = 0;
    }

    //LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
    return res;
}

ParseResult SourceChannel::recvRest (byte_t by, msize_t posBuff, msize_t posPacket)
{
    //LOG_LINE_GLOBAL("SServerClient", "---> posBuff:", posBuff, " posPacket:", posPacket, " g_posPayload:", g_posPayload, " _lenPayload:", _lenPayload);

    ParseResult res = ParseResult::ERROR;
    if ((posPacket >= g_posPayload) && (posPacket < g_posPayload + _lenPayload + g_lenCRC - 1))
        res = ParseResult::CONTINUE;
    else if (posPacket == g_posPayload + _lenPayload + g_lenCRC  - 1)
        res = ParseResult::FINISH;

    //LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
    return res;
}


SocketResult SourceChannel::recvPacket(StreamPacket &packet)
{
    return recv(packet);
}

SocketResult SourceChannel::recv(StreamPacket &packet)
{
    LOG_LINE_GLOBAL("SServerClient", "--->");

    const int                maxBuffLen = 1024;
    static __thread  char    buff[maxBuffLen];
    static __thread  msize_t lenBuff    = 0;
    static __thread  msize_t posBuff    = 0;
    static __thread  msize_t posPacket  = 0;


    struct parserFunc
    {
        parser_t    _fp;
        const char *_name;
    };
    parserFunc parserTable[] =  { { &SourceChannel::recvScanMID,            "ScanMID"       },
                                  { &SourceChannel::recvParseDataLength,    "DataLength"    },
                                  { &SourceChannel::recvRest,               "Rest"          },
                                  { nullptr, "" }
                                };
    static __thread  parserFunc *parsFunc = parserTable;


    SocketResult result = SocketResult::SR_ERROR_AGAIN;

    byte_t *packetData = nullptr;
    packet.Buffer(&packetData);

    while(result == SocketResult::SR_ERROR_AGAIN)
    {
        if (lenBuff == 0) //Buffer Consumed or empty
        {
            //LOG_LINE_GLOBAL("SServerClient", "recvFrom --->");
            lenBuff  = Read(buff, maxBuffLen);
            //LOG_LINE_GLOBAL("SServerClient", "recvFrom ---<   buff:", std::string(buff, lenBuff));
        }
        if (lenBuff > 0 && lenBuff < maxBuffLen + 1)
        {
            for (; (posBuff < lenBuff) && parsFunc->_fp && (result == SocketResult::SR_ERROR_AGAIN); ++posBuff)
            {
                ParseResult res = (this->*(parsFunc->_fp))(packetData[posPacket] = buff[posBuff], posBuff, posPacket);
                LOG_LINE_GLOBAL("SServerClient" , "_fp:",        parsFunc->_name
                                                , " by:",        packetData[posPacket]
                                                , "(0x",         std::hex, (int)packetData[posPacket], std::dec, ")"
                                                , " posBuff:",   posBuff
                                                , " posPacket:", posPacket
                                                , " res:",       ParseResultText(res));

                switch (res)
                {
                    case ParseResult::CONTINUE  :
                        posPacket++;
                        break;

                    case ParseResult::NEXT      :
                        posPacket++;
                        parsFunc++;
                        break;

                    case ParseResult::ERROR     :
                        parsFunc  = parserTable;
                        posPacket = 0;
                        posBuff   = _posBuffBeginPack;
                        break;

                    case ParseResult::FINISH    :
                        parsFunc  = parserTable;
                        posPacket = 0;

                        packet.BufferLen(posPacket);
                        if (true == packet.Check())
                        {
                            LOG_LINE_GLOBAL("SServerClient", "*****PACKET RECIEVED*****");
                            result = SocketResult::SR_SUCCESS;
                        }
                        else
                        {
                            LOG_LINE_GLOBAL("SServerClient", "*****PACKET FAILED*****");
                            posBuff   = _posBuffBeginPack;
                        }

                        break;
                }
            }
            if (posBuff == lenBuff)
            {
                LOG_LINE_GLOBAL("SServerClient", "recv buffuer consumed .....");
                posBuff = 0L;
                lenBuff = 0L;
            }
        }
    }
    LOG_LINE_GLOBAL("SServerClient", "---<");
    return result;
}



void SourceChannel::operator()()
{
    LOG_LINE_GLOBAL("SServerClient", "Source Listener Thread Entered");
    std::uint64_t count = 0;
    if (SocketResult::SR_SUCCESS != InitServer())
    {
        std::cerr << "Unable To initialize DomainSocket Server (" << strerror(errno) << ". - " << errno << ")\n";
    }
    else
    {

        while (true)
        {
            SocketDomain client = SocketDomain::Accept();

   //         if (sockClient != -1)
            {
                StreamPacket pck;
                recvPacket(pck);

                byte_t by[g_lenMaxPayload];
                int len = pck.Payload(by, g_lenMaxPayload);
                std::cout << count++ << ". : ("  << len << ") " << std::string((char*)by, len) << std::endl;
            }
        }
    }

    LOG_LINE_GLOBAL("SServerClient", "Source Listener Thread Quited");
}


