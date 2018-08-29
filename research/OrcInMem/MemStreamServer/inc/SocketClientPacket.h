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
#include "SocketUtils.h"



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

    virtual void OnRecvPacket(StreamPacket &&packet) = 0;
//    {
//        std::cout << "SocketClientPacket::OnRecvPacket -  Len = " << packet.PayloadLen() << std::endl;
//    }

    virtual void OnRecv(MemStream<std::uint8_t> &&stream)
    {
        LOG_LINE_GLOBAL("ServerEcho");

        std::cout << stream.dump("SocketClientPacket::OnRecv");

        SocketResult res = SocketResult::SR_ERROR_AGAIN;
        while(SocketResult::SR_ERROR_AGAIN == res)
        {
            StreamPacket packet;

            auto reader = [&stream] (char *buff, int offset, int len) -> int { return stream.read(buff, len, offset); };
            res = recvPacket(packet, reader);

            switch(res)
            {
                case SocketResult::SR_ERROR_AGAIN   :
                case SocketResult::SR_SUCCESS       :
                    OnRecvPacket(std::move(packet));
                    break;

                case SocketResult::SR_EMPTY         :
                    break;

                default                             :
                    OnErrorClient(res);
                    break;
            }
        }
    }
    virtual void OnErrorClient (SocketResult)
    {

    }







    ParseResult  recvScanMID         (byte_t by, msize_t posBuff, msize_t posPacket)
    {
        ParseResult res = ParseResult::ERROR;
        if ((posPacket >= g_posMID) && posPacket < g_posMID + g_lenMID)
        {
            if (posPacket == 0)
                _posBuffBeginPack = posBuff;
            if (by == StreamPacket::s_mid[posPacket])
                res = (posPacket == (g_posMID + g_lenMID - 1)) ? ParseResult::NEXT : ParseResult::CONTINUE;
        }
        return res;
    }

    ParseResult  recvParseDataLength (byte_t by, msize_t posBuff, msize_t posPacket)
    {
        ParseResult res = ParseResult::ERROR;
        if ((posPacket >= g_posPLen) && (posPacket < g_posPLen + g_lenPLen))
        {
            _lenghtBuff[_lenghtBuffPos++] = by;
            if (posPacket == g_posPLen + g_lenPLen - 1)
            {
                _lenPayload    = 0L;
                _lenghtBuffPos = 0L;

                memcpy(&_lenPayload, _lenghtBuff, g_lenPLen);
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
        return res;
    }

    ParseResult  recvPayload (byte_t by, msize_t posBuff, msize_t posPacket)
    {
        ParseResult res = ParseResult::ERROR;
        if ((posPacket >= g_posPayload) && (posPacket < g_posPayload + _lenPayload + g_lenCRC - 1))
            res = ParseResult::CONTINUE;
        else if (posPacket == g_posPayload + _lenPayload + g_lenCRC  - 1)
            res = ParseResult::FINISH;
        return res;
    }

    template <typename FunctorRead>
    SocketResult recvPacket(StreamPacket &packet, FunctorRead && reader)
    {
        const int                maxBuffLen           = 1024;
        static __thread  msize_t lenBuff              = 0;
        static __thread  msize_t posBuff              = 0;
        static __thread  msize_t offsetBuff           = 0;
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
                lenBuff = reader(buff, offsetBuff, maxBuffLen);
                offsetBuff += lenBuff;
            }
            if (lenBuff == 0)
            {
                result = SocketResult::SR_EMPTY;
                offsetBuff = 0;
                isDone = true;
            }
            else if (lenBuff > 0 && lenBuff < maxBuffLen + 1)
            {
                for (; (posBuff < lenBuff) && parsFunc->_fp && (false == isDone); ++posBuff)
                {
//                    std::cout << "1.  - "/* LOG_LINE_GLOBAL("SServerClient" */
//                                                    /*,*/ << "_fp:"        /*,*/  <<  parsFunc->_name
//                                                    /*,*/ << " by:"        /*,*/  <<  packetData[posPacket]
//                                                    /*,*/ << "(0x"         /*,*/  <<  std::hex  /*,*/<< (int)packetData[posPacket]  /*,*/<< std::dec /*,*/<< ")"
//                                                    /*,*/ << " posBuff:"   /*,*/  <<  posBuff
//                                                    /*,*/ << " lenBuff:"   /*,*/  <<  lenBuff
//                                                    /*,*/ << " posPacket:" /*,*/  <<  posPacket
//                                                          << std::endl;

                    ParseResult res = (this->*(parsFunc->_fp))(packetData[posPacket] = buff[posBuff], posBuff, posPacket);

//                    std::cout << "2.  - "/* LOG_LINE_GLOBAL("SServerClient" */
//                                                    /*,*/ << " res:"       /*,*/  <<  ParseResultText(res)   /*)*/
//                                                          << std::endl;


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

                            if (true == packet.Check())
                            {
                                result = SocketResult::SR_ERROR_AGAIN;
                                isDone = true;
                            }
                            else
                            {
                                //LOG_LINE_GLOBAL("SServerClient", "ERROR : PACKET CRC Mismatch (", parsFunc->_name, ")");
                                std::cerr << "ERROR : PACKET CRC Mismatch (" << parsFunc->_name << ")" << std::endl;
                                result = SocketResult::SR_ERROR_CRC;
                                posBuff   = _posBuffBeginPack;
                            }

                            parsFunc  = parserTable;
                            posPacket = 0;
                            break;
                    }
                }
            }

            if (posBuff == lenBuff)
            {
                posBuff = 0L;
                lenBuff = 0L;
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
                                  { ._fp = &SocketClientPacket::recvScanMID         ,   ._name = "ScanMID"       },
                                  { ._fp = &SocketClientPacket::recvParseDataLength ,   ._name = "DataLength"    },
                                  { ._fp = &SocketClientPacket::recvPayload         ,   ._name = "Payload"       },
                                  { ._fp = nullptr                                  ,   ._name = "nullptr"       }
                               };
};

#endif // __SOCKET_CLIENT_PACKET_H__
