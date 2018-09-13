/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcMemStreamRemoteSender.h
 * Author: can.gursu
 *
 * Created on 17 July 2018, 12:02
 */

#ifndef __ORC_MEM_STREAM_REMOTE_SENDER_H__
#define __ORC_MEM_STREAM_REMOTE_SENDER_H__

#include "SocketServer.h"
#include "SocketDomain.h"
#include "SocketClientPacket.h"
//#include "StreamPacket.h"
//#include "LoggerGlobal.h"


#include "orc/OrcFile.hh"





class OrcMemClient : public SocketClientPacket<SocketDomain>
{
    public:
        OrcMemClient(const char *name)
            : SocketClientPacket<SocketDomain>(name)
        {
        }
        
        virtual void OnRecvPacket(StreamPacket &&packet)
        {
            std::cout << "SocketClientPacket::OnRecvPacket -  Len = " << packet.PayloadLen() << std::endl;
        }
};

class OrcMemStreamRemoteSender
    : public orc::OutputStream
    , public orc::InputStream
    , public SocketServer<SocketDomain, OrcMemClient>
{
        using byte_t  = StreamPacket::byte_t;
        using SockSrv = SocketServer<SocketDomain, OrcMemClient>;
public:
    OrcMemStreamRemoteSender(const std::string &name = "OrcMem",
                             const std::string &memserChan = "/home/postgres/.sock_pgext_domain");
    OrcMemStreamRemoteSender(const OrcMemStreamRemoteSender& orig) = delete;
    OrcMemStreamRemoteSender(OrcMemStreamRemoteSender&& orig)      = delete;
    virtual ~OrcMemStreamRemoteSender();

    // From orc::OutputStream  &&  orc::InputStream
    virtual uint64_t           getLength() const;
    virtual uint64_t           getNaturalReadSize() const;
    virtual uint64_t           getNaturalWriteSize() const;
    virtual void               read(void* buf, uint64_t length, uint64_t offset);
    virtual void               write(const void* buf, size_t length);
    virtual const std::string& getName() const;
    virtual void               close();

    SocketResult               init();
    bool                       validateName();
    std::uint32_t              reqID();
    StreamPacket               packet();

    virtual void OnAccept      (const OrcMemClient &, const sockaddr &)
    {
        LOG_LINE_GLOBAL("remote", "XXXXXXXXXXXXXXXXXXXXXXX");
    }
    /*
    virtual void OnRecv        (OrcMemClient &,       MemStream<std::uint8_t> &&)
    {
        LOG_LINE_GLOBAL("remote", "XXXXXXXXXXXXXXXXXXXXXXX");
    }
    */
    virtual void OnDisconnect  (const OrcMemClient &)
    {
        LOG_LINE_GLOBAL("remote", "XXXXXXXXXXXXXXXXXXXXXXX");
    }
    virtual void OnErrorClient (const OrcMemClient &, SocketResult)
    {
        LOG_LINE_GLOBAL("remote", "XXXXXXXXXXXXXXXXXXXXXXX");
    }
    virtual void OnErrorServer (SocketResult)
    {
        LOG_LINE_GLOBAL("remote", "XXXXXXXXXXXXXXXXXXXXXXX");
    }

private:

    std::string _name;
    std::string _serverChannel;
};


#endif /* __ORC_MEM_STREAM_REMOTE_SENDER_H__ */

