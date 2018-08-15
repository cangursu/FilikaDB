/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SocketClientPacket.h
 * Author: can.gursu
 *
 * Created on 15 Agustos 2018, 12:00
 */


#ifndef __SOCKET_CLIENT_PACKET_H__
#define __SOCKET_CLIENT_PACKET_H__

#include "StreamPacket.h"
#include "SocketClient.h"
#include "ParseResult.h"
#include "LoggerGlobal.h"



template <typename TSocket>
class SocketClientPacket : public SocketClient<TSocket>
{
protected:
    using byte_t   = StreamPacket::byte_t;
    using msize_t  = StreamPacket::msize_t;
    using parser_t = ParseResult (SocketClientPacket::*)(byte_t, msize_t, msize_t) ;

public:

    SocketClientPacket() : SocketClient<TSocket>("SocketClientPacket")
    {
    }

    SocketClientPacket(const char *name) : SocketClient<TSocket>(name)
    {
    }

    /*
    SocketClientPacket(const char *name, const char *sourceChannelPath)
        : SocketClient<TSocket>(name)
    {
            SocketClient<TSocket>::SocketPath(sourceChannelPath);
    }
    */

    SocketClientPacket(int fd, const char *name) : SocketClient<TSocket>(fd, name)
    {
    }

    SocketClientPacket(const SocketClientPacket &val) = delete;
    //SocketClientPacket(const SocketDomain &val) = delete;

    SocketClientPacket(SocketClientPacket &&val)
        : SocketClient<TSocket>(std::move(val)/*, "SocketClientPacket"*/)
    {
    }


    SocketClientPacket& operator=(SocketClientPacket &&s)
    {
        SocketClient<TSocket>::operator = (std::move(s));
    }
    SocketClientPacket& operator=(const SocketClientPacket &val) = delete;


    SocketResult SendPacket(StreamPacket &packet)
    {
        StreamPacket::byte_t *  p   = nullptr;
        std::uint32_t           l   = packet.Buffer(&p);
        SocketResult            res = SocketClient<TSocket>::Send(p, l);

        if (SocketResult::SR_SUCCESS != res)
            OnErrorClient(res);

        return res;
    }

    virtual void OnRecvPacket(StreamPacket &&packet)
    {
    }

    virtual void OnRecv        (MemStream<std::uint8_t> &&stream)
    {
        {
            msize_t offsetStream = 0L;
            msize_t offsetPacket = 0L;
            //msize_t stLen        = stream.Len();
            //msize_t pyLen        = 0L;
            //msize_t pkLen        = 0L;

            //const msize_t        buffLen = 128;
            //StreamPacket::byte_t buff [buffLen];

            LOG_LINE_GLOBAL("ServerEcho");

            SocketResult res = SocketResult::SR_ERROR_AGAIN;
            while(SocketResult::SR_ERROR_AGAIN == res)
            {
                StreamPacket packet;

                auto reader = [&stream, &offsetStream] (char *buff, int len) -> int { return stream.read(buff, len, offsetStream); };
                res = recvPacket(packet, reader);

                //LOG_LINE_GLOBAL("ServerEcho", "recvPAcket : ", SocketResultText(res));
                if (res == SocketResult::SR_ERROR_AGAIN || res == SocketResult::SR_SUCCESS)
                {
                    OnRecvPacket(std::move(packet));
                    /*
                    pkLen = packet.BufferLen();
                    offsetStream += pkLen;

                    pyLen = packet.PayloadLen();
                    //LOG_LINE_GLOBAL("ServerEcho", "stLen:", stLen, " pkLen:", pkLen, ", pyLen:", pyLen, ", offsetStream:", offsetStream);
                    for (msize_t i = 0; i < pyLen; i += buffLen)
                    {
                        if (packet.PayloadPart(buff, buffLen, i) > 0)
                        {
                            LOG_LINE_GLOBAL("ServerEcho", "Packet:", std::string((char*)buff, pyLen));
                            std::cout << "PacketReveived:" << std::string((char*)buff, pyLen) << std::endl;
                        }
                    }
                    */
                }
                else
                {
                    OnErrorClient(res);
                }
            }
        }

    }
    virtual void OnErrorClient (SocketResult)
    {

    }







    ParseResult  recvScanMID         (byte_t by, msize_t posBuff, msize_t posPacket)
    {
        //    LOG_LINE_GLOBAL("SServerClient", "---> by:", by, "(", std::hex, by, std::dec, ")", " posBuff:", posBuff, " posPacket:", posPacket);
        ParseResult res = ParseResult::ERROR;
        if ((posPacket >= g_posMID) && posPacket < g_posMID + g_lenMID)
        {
            if (posPacket == 0)
                _posBuffBeginPack = posBuff;
            if (by == StreamPacket::s_mid[posPacket])
                res = (posPacket == (g_posMID + g_lenMID - 1)) ? ParseResult::NEXT : ParseResult::CONTINUE;
        }
        //    LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
        return res;
    }

    ParseResult  recvParseDataLength (byte_t by, msize_t posBuff, msize_t posPacket)
    {
        //    LOG_LINE_GLOBAL("SServerClient", "---> posBuff:", posBuff, " posPacket:", posPacket);
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
        //    LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
        return res;
    }

    ParseResult  recvRest            (byte_t by, msize_t posBuff, msize_t posPacket)
    {
        //    LOG_LINE_GLOBAL("SServerClient", "---> posBuff:", posBuff, " posPacket:", posPacket, " g_posPayload:", g_posPayload, " _lenPayload:", _lenPayload);
        ParseResult res = ParseResult::ERROR;
        if ((posPacket >= g_posPayload) && (posPacket < g_posPayload + _lenPayload + g_lenCRC - 1))
            res = ParseResult::CONTINUE;
        else if (posPacket == g_posPayload + _lenPayload + g_lenCRC  - 1)
            res = ParseResult::FINISH;
        //    LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
        return res;
    }

    template <typename FunctorRead>
    SocketResult recvPacket(StreamPacket &packet, FunctorRead && reader)
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

    //                std::cout /* LOG_LINE_GLOBAL("SServerClient" */
    //                                                /*,*/ << "_fp:"        /*,*/  <<  parsFunc->_name
    //                                                /*,*/ << " by:"        /*,*/  <<  packetData[posPacket]
    //                                                /*,*/ << "(0x"         /*,*/  <<  std::hex  /*,*/<< (int)packetData[posPacket]  /*,*/<< std::dec /*,*/<< ")"
    //                                                /*,*/ << " posBuff:"   /*,*/  <<  posBuff
    //                                                /*,*/ << " posPacket:" /*,*/  <<  posPacket
    //                                                /*,*/ << " res:"       /*,*/  <<  ParseResultText(res)   /*)*/<<std::endl;

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
                            //LOG_LINE_GLOBAL("SServerClient", "ERROR : PACKET ParseError (", parsFunc->_name, ")");
                            std::cerr << "ERROR : PACKET ParseError (" << parsFunc->_name << ")" << std::endl;
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
                                //LOG_LINE_GLOBAL("SServerClient", "PACKET RECIEVED .....");
                                //LOG_LINE_GLOBAL("SServerClient", "posBuff:", posBuff, " lenBuff:", lenBuff/*, " offset:", offset*/);
                                result = SocketResult::SR_ERROR_AGAIN;
                                isDone = true;
                            }
                            else
                            {
                                //LOG_LINE_GLOBAL("SServerClient", "ERROR : INVALID Packet");
                                posBuff   = _posBuffBeginPack;
                            }
                            break;
                    }
                }
            }
            if (posBuff == lenBuff)
            {
                //LOG_LINE_GLOBAL("SServerClient", "recv buffer consumed .....");
                //LOG_LINE_GLOBAL("SServerClient", "posBuff:", posBuff, " lenBuff:", lenBuff/*, " offset:", offset*/);

                result  = lenBuff > 0 ? SocketResult::SR_SUCCESS : SocketResult::SR_ERROR_EOF;
                //result  = SocketResult::SR_SUCCESS;

                posBuff = 0L;
                lenBuff = 0L;
                isDone  = true;
            }
        }
        return result;
    }


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
                                  { ._fp = &SocketClientPacket::recvScanMID,            ._name = "ScanMID"       },
                                  { ._fp = &SocketClientPacket::recvParseDataLength,    ._name = "DataLength"    },
                                  { ._fp = &SocketClientPacket::recvRest,               ._name = "Rest"          },
                                  { ._fp = nullptr, "" }
                               };
};


// .....    template <typename FunctorRead>
// .....    SocketResult SocketClientPacket::recvPacket(StreamPacket &packet, /*const MemStream<uint8_t> &stream, msize_t offset, */FunctorRead && reader)
// .....    {
// .....        const int                maxBuffLen           = 1024;
// .....        static __thread  msize_t lenBuff              = 0;
// .....        static __thread  msize_t posBuff              = 0;
// .....        static __thread  msize_t posPacket            = 0;
// .....        static __thread  const   parserFunc *parsFunc = parserTable;
// .....        static __thread  char    buff[maxBuffLen];
// .....
// .....
// .....        byte_t *packetData = nullptr;
// .....        packet.Buffer(&packetData);
// .....
// .....
// .....        bool         isDone = false;
// .....        SocketResult result = SocketResult::SR_ERROR_AGAIN;
// .....        while(false == isDone)
// .....        {
// .....            if (lenBuff == 0) //Buffer Consumed or empty
// .....            {
// .....                lenBuff = reader(buff, maxBuffLen);
// .....            }
// .....            if (lenBuff > 0 && lenBuff < maxBuffLen + 1)
// .....            {
// .....                for (; (posBuff < lenBuff) && parsFunc->_fp && (false == isDone); ++posBuff)
// .....                {
// .....                    ParseResult res = (this->*(parsFunc->_fp))(packetData[posPacket] = buff[posBuff], posBuff, posPacket);
// .....
// .....    //                std::cout /* LOG_LINE_GLOBAL("SServerClient" */
// .....    //                                                /*,*/ << "_fp:"        /*,*/  <<  parsFunc->_name
// .....    //                                                /*,*/ << " by:"        /*,*/  <<  packetData[posPacket]
// .....    //                                                /*,*/ << "(0x"         /*,*/  <<  std::hex  /*,*/<< (int)packetData[posPacket]  /*,*/<< std::dec /*,*/<< ")"
// .....    //                                                /*,*/ << " posBuff:"   /*,*/  <<  posBuff
// .....    //                                                /*,*/ << " posPacket:" /*,*/  <<  posPacket
// .....    //                                                /*,*/ << " res:"       /*,*/  <<  ParseResultText(res)   /*)*/<<std::endl;
// .....
// .....                    switch (res)
// .....                    {
// .....                        case ParseResult::CONTINUE  :
// .....                            posPacket++;
// .....                            break;
// .....
// .....                        case ParseResult::NEXT      :
// .....                            posPacket++;
// .....                            parsFunc++;
// .....                            break;
// .....
// .....                        case ParseResult::ERROR     :
// .....                            //LOG_LINE_GLOBAL("SServerClient", "ERROR : PACKET ParseError (", parsFunc->_name, ")");
// .....                            std::cerr << "ERROR : PACKET ParseError (" << parsFunc->_name << ")" << std::endl;
// .....                            parsFunc  = parserTable;
// .....                            posPacket = 0;
// .....                            posBuff   = _posBuffBeginPack;
// .....                            break;
// .....
// .....                        case ParseResult::FINISH    :
// .....                            packet.BufferLen(++posPacket);
// .....
// .....                            parsFunc  = parserTable;
// .....                            posPacket = 0;
// .....
// .....                            if (true == packet.Check())
// .....                            {
// .....                                //LOG_LINE_GLOBAL("SServerClient", "PACKET RECIEVED .....");
// .....                                //LOG_LINE_GLOBAL("SServerClient", "posBuff:", posBuff, " lenBuff:", lenBuff/*, " offset:", offset*/);
// .....                                result = SocketResult::SR_ERROR_AGAIN;
// .....                                isDone = true;
// .....                            }
// .....                            else
// .....                            {
// .....                                //LOG_LINE_GLOBAL("SServerClient", "ERROR : INVALID Packet");
// .....                                posBuff   = _posBuffBeginPack;
// .....                            }
// .....                            break;
// .....                    }
// .....                }
// .....            }
// .....            if (posBuff == lenBuff)
// .....            {
// .....                //LOG_LINE_GLOBAL("SServerClient", "recv buffer consumed .....");
// .....                //LOG_LINE_GLOBAL("SServerClient", "posBuff:", posBuff, " lenBuff:", lenBuff/*, " offset:", offset*/);
// .....
// .....                result  = lenBuff > 0 ? SocketResult::SR_SUCCESS : SocketResult::SR_ERROR_EOF;
// .....                //result  = SocketResult::SR_SUCCESS;
// .....
// .....                posBuff = 0L;
// .....                lenBuff = 0L;
// .....                isDone  = true;
// .....            }
// .....        }
// .....        return result;
// .....    }




#endif // __SOCKET_CLIENT_PACKET_H__
