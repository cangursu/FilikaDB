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

#include <thread>

class SourceChannel;

typedef  StreamPacket::byte_t  byte_t;
typedef  std::uint32_t         msize_t;
typedef  ParseResult (SourceChannel::*parser_t)(byte_t, msize_t, msize_t);


class SourceChannel : public SockDomain
{

    //using byte_t   = StreamPacket::byte_t;
    //using msize_t  = std::uint32_t;
    //using parser_t = ParseResult (SourceChannel::*)(byte_t, msize_t);
    //using ParseResult (SourceChannel::*parser_t)(byte_t, msize_t);

public:
    SourceChannel(const char *sourceChannelName);

    SocketResult recv(StreamPacket &packet);

    ParseResult recvScanMID         (byte_t by, msize_t posBuff, msize_t posPacket);
    ParseResult recvParseDataLength (byte_t by, msize_t posBuff, msize_t posPacket);
    ParseResult recvRest            (byte_t by, msize_t posBuff, msize_t posPacket);

    SocketResult recvPacket(StreamPacket &packet);

    static /*std::thread&& */ void listenAsyc(const char *sourceChannelName);


    msize_t _posBuffBeginPack = 0;
    msize_t _lenghtBuffPos    = 0;
    byte_t  _lenghtBuff[g_lenPLen];
    msize_t _lenPayload       = 0;


    /*
    SocketResult recv(std::string &item)
    {
        static __thread  char buff[1024];

        SocketResult res = SocketResult::ERROR;
        int len = recvFrom(buff, 1024);
        if (len > 0 && len < 1024)
        {
            res = SocketResult::SUCCESS;
            item.assign(buff, len);
        }
        return res;
    }
*/
    void operator()();
};

#endif // __SOURCE_CHANNEL_H__

