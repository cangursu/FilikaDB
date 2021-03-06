/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef __SOCKET_UTILITIES_H__
#define __SOCKET_UTILITIES_H__


#include "SocketResult.h"
#include "ParseResult.h"
#include "StreamPacket.h"

#include <sys/socket.h>
#include <functional>
#include <cstdint>
#include <string>


//#define FOLLOW_PYLDATA_FLOW
//#define FOLLOW_RAWDATA_FLOW


bool                NameInfo            (const sockaddr &addr, std::string &host, std::string &serv);
std::string         DumpMemory          (const void *p,         std::uint32_t len);
std::string         DumpMemory          (const std::uint8_t *p, std::uint32_t len);
std::string         DumpMemory          (const char *p,         std::uint32_t len);

const char *        ErrnoText           (int val);

const char *        EPollEventsText     (std::uint32_t eid);
std::string         EPollEvents         (std::uint32_t eid);

const char *        SocketResultText    (const SocketResult &val);
const char *        ParseResultText     (const ParseResult  &val);



void EncodeBase64(const MemStream<StreamPacket::byte_t> &stream,   MemStream<StreamPacket::byte_t> &stream64);
void EncodeBase64(const MemStream<StreamPacket::byte_t> &stream,   char *buff64, std::uint64_t &buff64Size);
void DecodeBase64(const MemStream<StreamPacket::byte_t> &stream64, MemStream<StreamPacket::byte_t> &stream);
void DecodeBase64(const MemStream<StreamPacket::byte_t> &stream64, char *buffer, std::uint64_t &bufferSize);


#endif //__SOCKET_UTILITIES_H__