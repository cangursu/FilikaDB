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
#include "crc32.h"

#include <utility>
#include <cstring>
#include <iostream>


const std::int32_t  StreamPacket::s_lenMaxPayload = _16K - 12;;
const std::int32_t  StreamPacket::s_lenMID        = 4;
const std::int32_t  StreamPacket::s_lenWidth      = 4;
const std::int32_t  StreamPacket::s_lenCRC        = 4;

const std::string   StreamPacket::s_mid           = "MYSF";


StreamPacket::StreamPacket()
{
}

StreamPacket::StreamPacket(const StreamPacket& orig)
{
}

StreamPacket::StreamPacket(StreamPacket&& orig)
{
}

StreamPacket::~StreamPacket()
{
}

void StreamPacket::Reset()
{
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
        chunkSize = (_K < bytesLeft) ? _K : bytesLeft;

        crc = crc32_fast(data + bytesProc, chunkSize, crc);
        bytesProc += chunkSize;
    }
    return crc;
}

bool StreamPacket::Check()
{
    StreamPacket::byte_t *ptWalk = _buff;

    byte_t mid[s_lenMID]   = "";
    std::uint32_t len = 0;

    std::memcpy(mid, (void*)ptWalk, s_lenMID);
    ptWalk += s_lenMID;
    std::memcpy((void*)&len, (void*)ptWalk, s_lenWidth);
    ptWalk += s_lenWidth;

    std::cout << "len : " << len << std::endl;
    byte_t pay[len];
    std::memcpy(pay, ptWalk, len);
    ptWalk += len;
    std::cout << "pay : " << pay << std::endl;

    std::uint32_t crc       = 0;
    std::memcpy(&crc, ptWalk, s_lenCRC);
    ptWalk += s_lenCRC;
    std::cout << "crc 1 : " << crc << std::endl;
    std::cout << "crc 2 : " << Crc(pay, len) << std::endl;

    return crc == Crc(pay, len);
}

SocketResult StreamPacket::Create(const byte_t *payload, std::uint32_t len)
{
    if (len > StreamPacket::s_lenMaxPayload) return SocketResult::ERROR_LEN;

    StreamPacket::byte_t *ptWalk = _buff;
    StreamPacket::byte_t *ptData = ptWalk;


    std::memcpy((void*)ptWalk, (void*)s_mid.c_str(), s_lenMID);
    ptWalk += s_lenMID;

    std::memcpy((void*)ptWalk, (void*)&len, s_lenWidth);
    ptWalk += s_lenWidth;

    std::memcpy(ptData = ptWalk, payload, len);
    ptWalk += len;


    std::uint32_t crc = Crc(ptData, len);
    std::cout << "crc : " << crc << std::endl;
    std::memcpy(ptWalk, &crc, s_lenCRC);
    ptWalk += s_lenCRC;

    return SocketResult::SUCCESS;
}