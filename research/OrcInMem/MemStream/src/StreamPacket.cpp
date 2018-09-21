/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   StreamPacket.cpp
 * Author: can.gursu
 *
 * Created on 17 July 2018, 14:36
 */

#include "StreamPacket.h"
#include "LoggerGlobal.h"
#include "crc32.h"
#include "SocketUtils.h"

#include <utility>
#include <cstring>
#include <iostream>


const std::string    StreamPacket::s_mid   = "MYSF";


StreamPacket::StreamPacket()
{
    //Reset();
}
/*
StreamPacket::StreamPacket(const StreamPacket& orig)
{
}

StreamPacket::StreamPacket(StreamPacket&& orig)
{
}
*/
StreamPacket::~StreamPacket()
{
}

void StreamPacket::Reset()
{
    std::memset(_buff, 0, g_lenMID + g_lenPLen);
    _buffLen = 0;
}

std::uint32_t StreamPacket::Crc(const void *data, std::uint32_t len) const
{
    std::uint32_t crc       = 0;
    std::size_t   bytesProc = 0;
    std::size_t   bytesLeft = 0;
    std::size_t   chunkSize = 0;

    while (bytesProc < len)
    {
        bytesLeft = len - bytesProc;
        chunkSize = (_1K < bytesLeft) ? _1K : bytesLeft;

        crc = crc32_fast(static_cast<const byte_t*>(data) + bytesProc, chunkSize, crc);
        bytesProc += chunkSize;
    }
    return crc;
}

bool StreamPacket::Check() const
{
//    std::cout << "CRC Test "  << std::endl;
//    std::cout << "_buffLen :" << _buffLen << " - _buff  :\n" << dumpMemory(_buff, _buffLen) << std::endl;



    const StreamPacket::byte_t *ptWalk = _buff;
    const StreamPacket::byte_t *ptData = nullptr;

    if (std::memcmp(ptWalk, s_mid.c_str(), g_lenMID))
        return false;
    ptWalk += g_lenMID;

    std::uint32_t len = 0;
    std::memcpy((void*)&len, (void*)ptWalk, g_lenPLen);
    ptWalk += g_lenPLen;
    if (len > g_lenMaxPayload)
        return false;

    ptData = ptWalk;
    ptWalk += len;

    std::uint32_t crc       = 0;
    std::memcpy(&crc, ptWalk, g_lenCRC);
    ptWalk += g_lenCRC;


//    std::cout << "CRC Test  crc : " << crc << " - Calculated : " << Crc(ptData, len) << std::endl;
    return crc == Crc(ptData, len);
}

std::uint32_t StreamPacket::Create(const byte_t *payload, std::uint32_t len)
{
    if (len > g_lenMaxPayload) return _buffLen = 0L;

    StreamPacket::byte_t *ptWalk = _buff;
    StreamPacket::byte_t *ptData = nullptr;

    std::memcpy((void*)ptWalk, (void*)s_mid.c_str(), g_lenMID);
    ptWalk += g_lenMID;

    std::memcpy((void*)ptWalk, (void*)&len, g_lenPLen);
    ptWalk += g_lenPLen;

    std::memcpy(ptData = ptWalk, payload, len);
    ptWalk += len;

    std::uint32_t crc = Crc(ptData, len);
    std::memcpy(ptWalk, &crc, g_lenCRC);
    ptWalk += g_lenCRC;

    //std::cout << "crc : " << crc << std::endl;
    return _buffLen = (ptWalk - _buff);
}

std::uint32_t StreamPacket::PayloadPart(byte_t *buff, std::uint32_t lenBuff, std::uint32_t offset) const
{
    if (false == Check()) return false;

    std::uint32_t         lenRead    = 0;
    std::uint32_t         lenPayload = 0;
    const StreamPacket::byte_t *ptWalk     = _buff  + g_lenMID;

    std::memcpy((void*)&lenPayload, (void*)ptWalk, g_lenPLen);
    ptWalk += g_lenPLen;

    if ((lenRead = lenPayload - offset) > lenBuff)
        lenRead = lenBuff;

    ptWalk += offset;
    std::memcpy(buff, ptWalk, lenRead);
    ptWalk += lenRead;

    return lenRead;
}

std::uint32_t StreamPacket::Payload(byte_t *buff, std::uint32_t lenBuff) const
{
    if (false == Check()) return false;

    std::uint32_t         len    = 0;
    const StreamPacket::byte_t *ptWalk = _buff  + g_lenMID;

    std::memcpy((void*)&len, (void*)ptWalk, g_lenPLen);
    ptWalk += g_lenPLen;
    if (len > lenBuff)
        return 0L;

    std::memcpy(buff, ptWalk, len);
    ptWalk += len;

    return len;
}

std::uint32_t StreamPacket::PayloadLen() const
{
    std::uint32_t         len    = 0;
    const StreamPacket::byte_t *ptWalk = _buff  + g_lenMID;

    std::memcpy((void*)&len, (void*)ptWalk, g_lenPLen);
    return len;
}


bool StreamPacket::operator == (const StreamPacket& orig) const
{
    return (_buffLen == orig._buffLen) && (0 == std::memcmp(_buff, orig._buff, _buffLen));
}



std::string StreamPacket::Dump(const std::string &msg /*= ""*/) const
{
    std::stringstream ss;

    ss << "Dump Packet --->  " << msg                          << std::endl;
    ss << "buffLen :  "        << _buffLen                     << std::endl;
    ss << "buff    : \n"       << dumpMemory(_buff, _buffLen)  << std::endl;

    return std::move(ss.str());
}

