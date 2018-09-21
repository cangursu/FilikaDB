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
#include <cstring>
#include <string>

#include <iostream>




#define  _1K   (1024)
#define  _4K   (4*_1K)
#define _16K   (16*_1K)


constexpr std::uint32_t  g_posMID              = 0;
constexpr std::uint32_t  g_lenMID              = 4;
constexpr std::uint32_t  g_posPLen             = g_posMID + g_lenMID;
constexpr std::uint32_t  g_lenPLen             = 4;
constexpr std::uint32_t  g_posPayload          = g_posPLen + g_lenPLen;
constexpr std::uint32_t  g_lenCRC              = 4;

constexpr std::uint32_t  g_lenOverhead         = g_lenCRC + g_lenPLen + g_lenMID;
constexpr std::uint32_t  g_lenMaxBuffer        = _16K;
constexpr std::uint32_t  g_lenMaxPayload       = g_lenMaxBuffer - g_lenOverhead;



class StreamPacket
{
public:
    using  byte_t  = std::uint8_t;
    using  msize_t = std::uint32_t;
public:
    virtual ~StreamPacket();

    StreamPacket();
    StreamPacket(const byte_t *payload, std::uint32_t len)  { Create(payload, len); }
    StreamPacket(const void   *payload, std::uint32_t len)  { Create(payload, len); }
    StreamPacket(const char   *payload)                     { Create(payload, std::strlen(payload)); }

    StreamPacket(const StreamPacket& orig)                       = default;
    StreamPacket(StreamPacket&& orig)                            = default;
    StreamPacket &  operator = (const StreamPacket& orig)        = default;
    StreamPacket &  operator = (StreamPacket&& orig)             = default;

    bool            operator == (const StreamPacket& orig) const;

    void            Reset();
    bool            Check() const;
    std::uint32_t   Crc(const void *data, std::uint32_t len) const;

    std::uint32_t   Create     (const byte_t *payload, std::uint32_t len);
    std::uint32_t   Create     (const void   *payload, std::uint32_t len)   { return Create((byte_t *)payload, len); }
    std::uint32_t   Payload    (byte_t       *buff   , std::uint32_t len) const;
    std::uint32_t   PayloadPart(byte_t       *buff   , std::uint32_t len, std::uint32_t offset) const;
    std::uint32_t   PayloadLen () const;

    std::uint32_t   Buffer     (byte_t **buff)                              { *buff = _buff; return _buffLen;  }
    std::uint32_t   Buffer     (byte_t const **buff) const                  { *buff = _buff; return _buffLen;  }

    void            BufferLen  (std::uint32_t len)                       { _buffLen = len;  }
    std::uint32_t   BufferLen  () const                                  { return _buffLen; }

    std::string     Dump       (const std::string &msg = "") const;
private:

    byte_t          _buff[g_lenMaxBuffer];
    std::uint32_t   _buffLen = 0;

public:

    static const std::string    s_mid;

};

#endif // __STREAM_PACKET_H__

