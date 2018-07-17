/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   StreamPacket.h
 * Author: can.gursu
 *
 * Created on 17 July 2018, 14:36
 */

#ifndef __STREAM_PACKET_H__
#define __STREAM_PACKET_H__


#include "SocketResult.h"

#include <cstdint>
#include <string>



#define _K      (1024)
#define _4K     (4*_K)
#define _16K   (16*_K)

class StreamPacket
{
    using  byte_t = std::uint8_t;
public:
    StreamPacket();
    StreamPacket(const StreamPacket& orig);
    StreamPacket(StreamPacket&& orig);
    virtual ~StreamPacket();

    void            Reset();
    bool            Check();
    std::uint32_t   Crc(const void *data, std::uint32_t len);

    SocketResult    Create(const byte_t *payload, std::uint32_t len);
    SocketResult    Create(const void   *payload, std::uint32_t len)  { return Create((byte_t *)payload, len); }

private:

    byte_t _buff[_16K];

    static const std::int32_t  s_lenMaxPayload;
    static const std::int32_t  s_lenMID       ;
    static const std::int32_t  s_lenWidth     ;
    static const std::int32_t  s_lenCRC       ;

    static const std::string   s_mid          ;

};

#endif // __STREAM_PACKET_H__

