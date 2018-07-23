/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcMemStreamRemoteSender.cpp
 * Author: can.gursu
 *
 * Created on 17 July 2018, 12:02
 */


#include "OrcMemStreamRemoteSender.h"


#include <string>
extern "C"
{
#include "postgres.h"
}


OrcMemStreamRemoteSender::OrcMemStreamRemoteSender(const std::string &name,
                                                   const std::string &serverChannel)
    : _name (name)
    , _serverChannel (serverChannel)
{
}

OrcMemStreamRemoteSender::~OrcMemStreamRemoteSender()
{
    //Assert(true);
}

int OrcMemStreamRemoteSender::init()
{
    int res = _sock.init(_serverChannel.c_str());
    //    LOG_LINE_GLOBAL("remote", "res = ", res);
    if (0 == res)
    {
    }
    return res;
}

std::uint32_t OrcMemStreamRemoteSender::reqID()
{
    return 12345; //TODO: Implement a unique Request ID
}


StreamPacket OrcMemStreamRemoteSender::packet()
{
    StreamPacket pack;
    //const char *p = "1234567890abcdefghijklmnopqrstuvwxyz";
    const char *p = "12345";
    int lll = pack.Create(p, std::strlen(p));

    return std::move(pack);
}

uint64_t OrcMemStreamRemoteSender::getLength() const
{
    //Assert(true);
    return 0L;
}

uint64_t OrcMemStreamRemoteSender::getNaturalReadSize() const
{
    //Assert(true);
    return 0L;
}

uint64_t OrcMemStreamRemoteSender::getNaturalWriteSize() const
{
    //Assert(true);
    return 0L;
}

void OrcMemStreamRemoteSender::read(void* buf, uint64_t length, uint64_t offset)
{
    //Assert(true);
}

void OrcMemStreamRemoteSender::write(const void* buf, size_t length)
{
    LOG_LINE_GLOBAL("remote", "xxx");
    _sock.send(std::move(StreamPacket(buf, length)));
    //Assert(true);
}

const std::string& OrcMemStreamRemoteSender::getName() const
{
    //Assert(true);
    return _name;
}

void OrcMemStreamRemoteSender::close()
{
    //Assert(true);
}

bool OrcMemStreamRemoteSender::validateName()
{
    if (_name.size() < 2) return false;
    if (_name.find('|') != std::string::npos) return false;
    return true;
}




bool OrcMemStreamRemoteSender::StreamCSock::send(const StreamPacket &pack)
{
    const byte_t *b;
    std::uint32_t l = pack.Buffer(&b);

    LOG_LINE_GLOBAL("remote", "l = ", l, " b = ", b);

    ssize_t res = (l > 0) ? sendTo(b, l) : 0;
    return ((res > 0) && (res == (ssize_t)l));
}
/*
bool OrcMemStreamRemoteSender::StreamCSock::send(const char *data, int len)
{
    ssize_t res = 0;
    if (len > 0) res = sendTo(data, len);
    return ((res > 0) && (res == (ssize_t)len));
}
*/


