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

#include "SocketDomain.h"
#include "LoggerGlobal.h"
#include "StreamPacket.h"

#include "orc/OrcFile.hh"



class OrcMemStreamRemoteSender
    : public orc::OutputStream
    , public orc::InputStream
{
        using byte_t = StreamPacket::byte_t;
public:
    OrcMemStreamRemoteSender(const std::string &name = "OrcMem",
                             const std::string &memserChan = "/home/postgres/pgext_domain_sock");
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

    int                        init();
    bool                       validateName();
    std::uint32_t              reqID();
    StreamPacket               packet();
private:


    class StreamCSock : public SockDomain
    {
    public :
        bool send(const StreamPacket &pack);
        //bool send(const char *data, int len);
    };


    StreamCSock _sock;
    std::string _name;
    std::string _serverChannel;
};

#endif /* __ORC_MEM_STREAM_REMOTE_SENDER_H__ */

