/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SourceChannel.h
 * Author: can.gursu
 *
 * Created on 19 July 2018, 15:22
 */

#ifndef __SOURCE_CHANNEL_H__
#define __SOURCE_CHANNEL_H__

#include "SocketDomain.h"
#include "StreamPacket.h"
#include "ParseResult.h"
#include "MemStream.h"
#include "LoggerGlobal.h"


#include <thread>

class SourceChannelClient;

typedef  StreamPacket::byte_t  byte_t;
typedef  std::uint32_t         msize_t;

typedef  ParseResult (SourceChannelClient::*parser_t)(byte_t, msize_t, msize_t) ;


class SourceChannelServer : public SocketDomain
{
public:
    SourceChannelServer(const char *sourceChannelPath )
        : SocketDomain("SourceChannelServer")
        , _sourceChannelName("SourceChannelServer")
    {
        SocketPath(sourceChannelPath);
    }

    static /*std::thread&& */ void listenAsyc(const char *sourceChannelName, bool async = false);
    void operator()();

    std::string _sourceChannelName;
};


class SourceChannelClient : public SocketDomain
{
public:


    SourceChannelClient()
    {
    }

    SourceChannelClient(const char *sourceChannelPath)
        : SocketDomain("SourceChannelClient")
    {
            SocketPath(sourceChannelPath);
    }

    SourceChannelClient(int fd, const char *name)
        : SocketDomain(fd, name/*"SourceChannelClient"*/)
    {
    }

    SourceChannelClient(const SocketDomain &val) = delete;

    SourceChannelClient(SocketDomain &&val)
        : SocketDomain(std::move(val)/*, "SourceChannelClient"*/)
    {
    }

    ParseResult  recvScanMID         (byte_t by, msize_t posBuff, msize_t posPacket) ;
    ParseResult  recvParseDataLength (byte_t by, msize_t posBuff, msize_t posPacket) ;
    ParseResult  recvRest            (byte_t by, msize_t posBuff, msize_t posPacket) ;

    template <typename FunctorRead>
    SocketResult recvPacket(StreamPacket &packet, FunctorRead && reader);

    //SocketResult recvPacket          (StreamPacket &packet, const MemStream<uint8_t> &stream, msize_t offset);
    //SocketResult recvPacket          (StreamPacket &packet);

    msize_t _posBuffBeginPack = 0;

    msize_t _lenghtBuffPos    = 0;
    byte_t  _lenghtBuff[g_lenPLen];

    msize_t _lenPayload       = 0;

//    StreamPacket packet;

private:

    struct parserFunc
    {
        parser_t    _fp;
        const char *_name;
    };
    parserFunc parserTable[4] {
                                  { ._fp = &SourceChannelClient::recvScanMID,            ._name = "ScanMID"       },
                                  { ._fp = &SourceChannelClient::recvParseDataLength,    ._name = "DataLength"    },
                                  { ._fp = &SourceChannelClient::recvRest,               ._name = "Rest"          },
                                  { ._fp = nullptr, "" }
                               };
};





template <typename FunctorRead>
SocketResult SourceChannelClient::recvPacket(StreamPacket &packet, /*const MemStream<uint8_t> &stream, msize_t offset, */FunctorRead && reader)
{
    const int                maxBuffLen           = 1024;
    static __thread  msize_t lenBuff              = 0;
    static __thread  msize_t posBuff              = 0;
    static __thread  msize_t posPacket            = 0;
    static __thread  const   parserFunc *parsFunc = parserTable;
    static __thread  char    buff[maxBuffLen];


    byte_t *packetData = nullptr;
    packet.Buffer(&packetData);


    bool         isDone = false;
    SocketResult result = SocketResult::SR_ERROR_AGAIN;
    while(false == isDone)
    {
        if (lenBuff == 0) //Buffer Consumed or empty
        {
            lenBuff = reader(buff, maxBuffLen);
        }
        if (lenBuff > 0 && lenBuff < maxBuffLen + 1)
        {
            for (; (posBuff < lenBuff) && parsFunc->_fp && (false == isDone); ++posBuff)
            {
                ParseResult res = (this->*(parsFunc->_fp))(packetData[posPacket] = buff[posBuff], posBuff, posPacket);
/*
                LOG_LINE_GLOBAL("SServerClient" , "_fp:",        parsFunc->_name
                                                , " by:",        packetData[posPacket]
                                                , "(0x",         std::hex, (int)packetData[posPacket], std::dec, ")"
                                                , " posBuff:",   posBuff
                                                , " posPacket:", posPacket
                                                , " res:",       ParseResultText(res));
*/
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
                        LOG_LINE_GLOBAL("SServerClient", "ERROR : PACKET ParseError (", parsFunc->_name, ")");
                        parsFunc  = parserTable;
                        posPacket = 0;
                        posBuff   = _posBuffBeginPack;
                        break;

                    case ParseResult::FINISH    :
                        packet.BufferLen(++posPacket);

                        parsFunc  = parserTable;
                        posPacket = 0;

                        if (true == packet.Check())
                        {
                            LOG_LINE_GLOBAL("SServerClient", "PACKET RECIEVED .....");
                            LOG_LINE_GLOBAL("SServerClient", "posBuff:", posBuff, " lenBuff:", lenBuff/*, " offset:", offset*/);
                            result = SocketResult::SR_ERROR_AGAIN;
                            isDone = true;
                        }
                        else
                        {
                            LOG_LINE_GLOBAL("SServerClient", "ERROR : INVALID Packet");
                            posBuff   = _posBuffBeginPack;
                        }
                        break;
                }
            }
        }
        if (posBuff == lenBuff)
        {
            LOG_LINE_GLOBAL("SServerClient", "recv buffer consumed .....");
            LOG_LINE_GLOBAL("SServerClient", "posBuff:", posBuff, " lenBuff:", lenBuff/*, " offset:", offset*/);

            result  = lenBuff > 0 ? SocketResult::SR_SUCCESS : SocketResult::SR_ERROR_EOF;
            //result  = SocketResult::SR_SUCCESS;

            posBuff = 0L;
            lenBuff = 0L;
            isDone  = true;
        }
    }
    return result;
}










#endif // __SOURCE_CHANNEL_H__

