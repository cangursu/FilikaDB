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


#include <iostream>




#define  _1K   (1024)
#define  _4K   (4*_1K)
#define _16K   (16*_1K)

const std::uint32_t  g_posMID         = 0;
const std::uint32_t  g_lenMID         = 4;
const std::uint32_t  g_posPLen        = g_posMID + g_lenMID;
const std::uint32_t  g_lenPLen        = 4;
const std::uint32_t  g_posPayload     = g_posPLen + g_lenPLen;
const std::uint32_t  g_lenCRC         = 4;

const std::uint32_t  g_lenMaxBuffer   = _16K;
const std::uint32_t  g_lenMaxPayload  = g_lenMaxBuffer - (g_lenCRC + g_lenPLen + g_lenMID);


class StreamPacket
{
public:
    using  byte_t  = std::uint8_t;
    using  msize_t = std::uint32_t;
public:
    virtual ~StreamPacket();

    StreamPacket();
    StreamPacket(const byte_t *payload, std::uint32_t len) { Create(payload, len); }
    StreamPacket(const void   *payload, std::uint32_t len) { Create(payload, len); }
    /*
    StreamPacket(const StreamPacket& orig)                  = default;
    StreamPacket(StreamPacket&& orig)                       = default;
    StreamPacket &  operator = (const StreamPacket& orig)   = default;
    StreamPacket && operator = (StreamPacket&& orig)        = default;
    */

    void            Reset();
    bool            Check();
    std::uint32_t   Crc(const void *data, std::uint32_t len);

    std::uint32_t   Create     (const byte_t *payload, std::uint32_t len);
    std::uint32_t   Create     (const void   *payload, std::uint32_t len)   { return Create((byte_t *)payload, len); }
    std::uint32_t   Payload    (byte_t       *buff   , std::uint32_t len);
    std::uint32_t   PayloadPart(byte_t       *buff   , std::uint32_t len, std::uint32_t offset);
    std::uint32_t   PayloadLen ();

    std::uint32_t   Buffer (byte_t **buff)                              { *buff = _buff; return _buffLen;  }
    std::uint32_t   Buffer (byte_t const **buff) const                  { *buff = _buff; return _buffLen;  }

    void            BufferLen (std::uint32_t len)                       { _buffLen = len;  }
    std::uint32_t   BufferLen () const                                  { return _buffLen; }

private:

    byte_t          _buff[g_lenMaxBuffer];
    std::uint32_t   _buffLen = 0;

public:

    static const std::string    s_mid;

};

#endif // __STREAM_PACKET_H__

