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
#include "GeneralUtils.h"



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
            this->OnErrorClient(res);
#ifdef FOLLOW_PYLDATA_FLOW
        else
        {
            std::cout << std::endl << packet.DumpPayload("SocketClientPacket::SendPacket ->") << std::endl;
        }
#endif  //FOLLOW_PYLDATA_FLOW

        return res;
    }

    //
    // Abstract Recieve Handler
    //
    virtual void OnRecvPacket(StreamPacket &&packet) = 0;


    virtual void OnRecv(MemStream<std::uint8_t> &&stream)
    {
#ifdef FOLLOW_RAWDATA_FLOW
        std::cout << stream.Dump("SocketClientPacket::OnRecv --->");
#endif //FOLLOW_RAWDATA_FLOW

        SocketResult res = SocketResult::SR_ERROR_AGAIN;
        while(SocketResult::SR_ERROR_AGAIN == res)
        {
            StreamPacket packet;

            auto reader = [&stream] (char *buff, int offset, int len) -> int { return stream.Read(buff, len, offset); };
            res = recvPacket(packet, reader);

//            std::cout << "SocketClientPacket::OnRecv recvPacket:" << SocketResultText(res) << std::endl;
//            std::cout << packet.DumpPayload("len : " + std::to_string(packet.PayloadLen())) << std::endl;

            switch(res)
            {
                case SocketResult::SR_ERROR_AGAIN   :
                case SocketResult::SR_SUCCESS       :
                    OnRecvPacket(std::move(packet));
                    break;

                case SocketResult::SR_EMPTY         :
                    break;

                default                             :
                    this->OnErrorClient(res);
                    break;
            }
        }

//        std::cout << "SocketClientPacket::OnRecv ---<" << std::endl;
    }
    /*
    virtual void OnErrorClient (SocketResult)
    {
    }
    */





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
//        std::cout << "SocketClientPacket::recvPacket --->\n";

        byte_t *packetData = nullptr;
        packet.Buffer(&packetData);


        bool         isDone = false;
        SocketResult result = SocketResult::SR_ERROR_AGAIN;
        while(false == isDone)
        {
            if (_lenBuff == 0) //Buffer Consumed or empty
            {
                _lenBuff     = reader(_buff, _offsetBuff, _maxBuffLen);
                _offsetBuff += _lenBuff;
            }
            if (_lenBuff == 0)
            {
                result = SocketResult::SR_EMPTY;
                _offsetBuff = 0;
                isDone = true;
            }
            else if (_lenBuff > 0 && _lenBuff < _maxBuffLen + 1)
            {
                for (; (_posBuff < _lenBuff) && _parsFunc->_fp && (false == isDone); ++_posBuff)
                {
//                    std::cout   << "1.  - "
//                                << "_fp:"           <<  _parsFunc->_name
//                                << " by:"           <<  _buff[_posBuff]
//                                << "(0x"            <<  std::hex << (int)_buff[_posBuff]        << std::dec << ")"
//                                << " - "            <<  packetData[_posPacket]
//                                << "(0x"            <<  std::hex << (int)packetData[_posPacket] << std::dec << ")"
//                                << " _posBuff:"     <<  _posBuff
//                                << " _lenBuff:"     <<  _lenBuff
//                                << " _posPacket:"   <<  _posPacket
//                                << std::endl;

                    ParseResult res = (this->*(_parsFunc->_fp))(packetData[_posPacket] = _buff[_posBuff], _posBuff, _posPacket);

//                    std::cout   << "2.  - "
//                                << " by:"           <<  packetData[_posPacket]
//                                << "(0x"            <<  std::hex << (int)packetData[_posPacket] << std::dec << ")"
//                                << " res:"          <<  ParseResultText(res)   /*)*/
//                                << std::endl;


                    switch (res)
                    {
                        case ParseResult::CONTINUE  :
                            _posPacket++;
                            break;

                        case ParseResult::NEXT      :
                            _posPacket++;
                            _parsFunc++;
                            break;

                        case ParseResult::ERROR     :
                            //LOG_LINE_GLOBAL("SServerClient", "ERROR : PACKET ParseError (", parsFunc->_name, ")");
                            std::cerr << "ERROR : PACKET ParseError (" << _parsFunc->_name << ")" << std::endl;
                            this->OnErrorClient(SocketResult::SR_ERROR_PARSE);
                            _parsFunc  = _parserTable;
                            _posPacket = 0;
                            _posBuff   = _posBuffBeginPack;
                            break;

                        case ParseResult::FINISH    :
                            packet.BufferLen(++_posPacket);

                            if (true == packet.Check())
                            {
#ifdef FOLLOW_PYLDATA_FLOW
                                std::cout << packet.DumpPayload("SocketClientPacket::recvPacket -> ") << std::endl;
#endif //FOLLOW_PYLDATA_FLOW

                                result = SocketResult::SR_ERROR_AGAIN;
                                isDone = true;
                            }
                            else
                            {
                                //LOG_LINE_GLOBAL("SServerClient", "ERROR : PACKET CRC Mismatch (", parsFunc->_name, ")");
                                std::cerr << "ERROR : PACKET CRC Mismatch (" << _parsFunc->_name << ")" << std::endl;
                                result     = SocketResult::SR_ERROR_CRC;
                                _posBuff   = _posBuffBeginPack;
                            }

                            _parsFunc  = _parserTable;
                            _posPacket = 0;
                            break;
                    }
                }
            }

            if (_posBuff == _lenBuff)
            {
                _posBuff = 0L;
                _lenBuff = 0L;
            }
        }

//        std::cout << "SocketClientPacket::recvPacket ---<\n";
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
    parserFunc _parserTable[4] {
                                  { ._fp = &SocketClientPacket::recvScanMID         ,   ._name = "ScanMID"       },
                                  { ._fp = &SocketClientPacket::recvParseDataLength ,   ._name = "DataLength"    },
                                  { ._fp = &SocketClientPacket::recvPayload         ,   ._name = "Payload"       },
                                  { ._fp = nullptr                                  ,   ._name = "nullptr"       }
                                };
private :
    msize_t             _lenBuff              = 0;
    msize_t             _posBuff              = 0;
    msize_t             _offsetBuff           = 0;
    msize_t             _posPacket            = 0;
    const parserFunc *  _parsFunc             = _parserTable;

    static const int    _maxBuffLen           = 1024;
    char                _buff[_maxBuffLen];

};

#endif // __SOCKET_CLIENT_PACKET_H__
