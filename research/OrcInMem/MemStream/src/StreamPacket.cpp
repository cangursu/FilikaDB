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

#include <utility>
#include <cstring>
#include <iostream>


/*
const std::uint32_t  StreamPacket::s_lenMID         = 4;
const std::uint32_t  StreamPacket::s_lenDLen        = 4;
const std::uint32_t  StreamPacket::s_lenCRC         = 4;

const std::uint32_t  StreamPacket::s_lenMaxBuffer   = _16K;
const std::uint32_t  StreamPacket::s_lenMaxPayload  = StreamPacket::s_lenMaxBuffer - (StreamPacket::s_lenCRC + StreamPacket::s_lenDLen + StreamPacket::s_lenMID);
*/
const std::string    StreamPacket::s_mid            = "MYSF";


StreamPacket::StreamPacket()
{
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
}

std::uint32_t StreamPacket::Crc(const void *data, std::uint32_t len)
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

bool StreamPacket::Check()
{
    StreamPacket::byte_t *ptWalk = _buff;
    StreamPacket::byte_t *ptData = nullptr;


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

std::uint32_t StreamPacket::Payload(byte_t *buff, std::uint32_t lenBuff)
{
    if (false == Check()) return false;

    std::uint32_t         len    = 0;
    StreamPacket::byte_t *ptWalk = _buff  + g_lenMID;

    std::memcpy((void*)&len, (void*)ptWalk, g_lenPLen);
    ptWalk += g_lenPLen;
    if (len > lenBuff)
        return 0L;

    std::memcpy(buff, ptWalk, len);
    ptWalk += len;

    return len;
}
