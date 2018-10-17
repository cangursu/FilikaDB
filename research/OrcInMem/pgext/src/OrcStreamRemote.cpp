/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcStreamRemote.cpp
 * Author: can.gursu
 *
 * Created on 17 July 2018, 12:02
 */


#include "OrcStreamRemote.h"
#include "MemStreamPacket.h"


#include <string>
extern "C"
{
#include "postgres.h"
}

OrcStreamRemote::OrcStreamRemote(const std::string &name)
    : SocketClientPacket<SocketTCP>(name.c_str())
    , _name (name)
{
}

OrcStreamRemote::~OrcStreamRemote()
{
    //Assert(true);
}

SocketResult OrcStreamRemote::init(const char *address, std::uint16_t post)
{
    Address("127.0.0.1", 5001);
    return init();
}

SocketResult OrcStreamRemote::init()
{
    SocketResult res;

    if (SocketResult::SR_SUCCESS != (res = Init()))
    {
        LOG_LINE_GLOBAL("remote", "ERROR: Unable to initialize OrcStreamRemote");
    }
    else if (SocketResult::SR_SUCCESS != (res = this->Connect()))
    {
        LOG_LINE_GLOBAL("remote", "ERROR: Unable to Connect SocketTCP - ", PrmDesc());
        LOG_LINE_GLOBAL("remote", PrmDesc());
    }

    return res;
}

std::uint64_t OrcStreamRemote::refid()
{
    return 12345; //TODO: Implement a unique Request ID
}


StreamPacket OrcStreamRemote::packet()
{
    StreamPacket pack;
    //const char *p = "1234567890abcdefghijklmnopqrstuvwxyz";
    const char *p = "12345";
    int lll = pack.Create(p, std::strlen(p));

    return std::move(pack);
}

uint64_t OrcStreamRemote::getLength() const
{
    //Assert(true);
    return 128L;
}

uint64_t OrcStreamRemote::getNaturalReadSize() const
{
    //Assert(true);
    return 128L;
}

uint64_t OrcStreamRemote::getNaturalWriteSize() const
{
    //Assert(true);
    return 128L;
}

void OrcStreamRemote::read(void* buf, uint64_t length, uint64_t offset)
{
    LOG_LINE_GLOBAL("remote", "************");
    LOG_LINE_GLOBAL("remote", "length:", length, " - offset:", offset);
    LOG_LINE_GLOBAL("remote", "************");
    //Assert(true);
}

void OrcStreamRemote::write(const void* buf, size_t length)
{
    LOG_LINE_GLOBAL("remote", "length=", length);

    MemStreamPacket packet;
    packet.CreatePacketWrite("TableXXX", buf, length);


    //MemStreamPacket::byte_t payload[2048] = "";
    //int len = packet.Payload(payload, 2048);
    //LOG_LINE_GLOBAL("remote", "len=", len);
    //LOG_LINE_GLOBAL("remote", "payload=", payload);

    if (SocketResult::SR_SUCCESS != SendPacket(packet))
    {
        LOG_LINE_GLOBAL("remote", "ERROR: Unable to send packet. errno:", errno);
        std::cerr <<  "ERROR : SendPacket failed.\n";
    }
}

const std::string& OrcStreamRemote::getName() const
{
    LOG_LINE_GLOBAL("remote", "");
    //Assert(true);
    return _name;
}

void OrcStreamRemote::close()
{
    //Assert(true);
}

bool OrcStreamRemote::validateName()
{
    if (_name.size() < 2) return false;
    if (_name.find('|') != std::string::npos) return false;
    return true;
}





/*
bool OrcStreamRemote::StreamCSock::send(const StreamPacket &pack)
{
    const byte_t *b;
    std::uint32_t l = pack.Buffer(&b);

    LOG_LINE_GLOBAL("remote", "l = ", l, " b = ", b);

    / *
    ssize_t res = (l > 0) ? sendTo(b, l) : 0;
    return ((res > 0) && (res == (ssize_t)l));
    * /
    return false;
}
*/
/*
bool OrcStreamRemote::StreamCSock::send(const char *data, int len)
{
    ssize_t res = 0;
    if (len > 0) res = sendTo(data, len);
    return ((res > 0) && (res == (ssize_t)len));
}
*/


