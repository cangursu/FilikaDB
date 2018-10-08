/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcStreamRemote.h
 * Author: can.gursu
 *
 * Created on 17 July 2018, 12:02
 */

#ifndef __ORC_MEM_STREAM_REMOTE_SENDER_H__
#define __ORC_MEM_STREAM_REMOTE_SENDER_H__

#include "SocketServer.h"
#include "SocketTCP.h"
#include "SocketClientPacket.h"
//#include "StreamPacket.h"
//#include "LoggerGlobal.h"


#include "orc/OrcFile.hh"
#include <sstream>




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
        virtual void OnErrorClient (SocketResult err)
        {
            std::cerr << "ERROR : OrcMemClient::OnErrorClient - " << SocketResultText(err) << std::endl;
        }
};

class OrcStreamRemote___IN___XXX___ :  public orc::InputStream, public SocketClientPacket<SocketTCP>
{
public:
    OrcStreamRemote___IN___XXX___(const std::string &name)
        : SocketClientPacket<SocketTCP>(name.c_str())
        , _name (name)
    {
    }
    virtual uint64_t getLength() const                              {return 128;}
    virtual uint64_t getNaturalReadSize() const                     {return 128;}
    virtual void read(void* buf,uint64_t length,uint64_t offset)    {return ;}
    virtual const std::string& getName() const                      {return _name;}
    virtual void OnRecvPacket(StreamPacket &&packet)                {return ;}
    virtual void OnErrorClient (SocketResult)                       {return ;}

    SocketResult init(const char *address, std::uint16_t post)
    {
        Address("127.0.0.1", 5001);
        return init();
    }

    SocketResult init()
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


private:
    std::string _name;
};


class OrcStreamRemote
    : public orc::OutputStream
    , public orc::InputStream
    , public SocketClientPacket<SocketTCP>
{
        using byte_t  = StreamPacket::byte_t;
public:
    OrcStreamRemote(const std::string &name = "OrcMem");
    OrcStreamRemote(const OrcStreamRemote& orig)        = delete;
    OrcStreamRemote(OrcStreamRemote&& orig)             = delete;
    virtual ~OrcStreamRemote();

    // From orc::OutputStream  &&  orc::InputStream
    virtual uint64_t           getLength() const;
    virtual uint64_t           getNaturalReadSize() const;
    virtual uint64_t           getNaturalWriteSize() const;
    virtual void               read(void* buf, uint64_t length, uint64_t offset);
    virtual void               write(const void* buf, size_t length);
    virtual const std::string& getName() const;
    virtual void               close();

    SocketResult               init();
    SocketResult               init(const char *address, std::uint16_t post);
    bool                       validateName();
    std::uint64_t              refid();
    StreamPacket               packet();



    // From SocketClientPacket
    inline void OnErrorClient (SocketResult err)
    {
        std::cout << "ERROR : SClient::OnErrorClient - " << SocketResultText(err) << std::endl;
    }

    inline void OnRecvPacket  (StreamPacket &&packet)
    {
        msize_t         pyLen     = packet.PayloadLen();
        msize_t         pyLenRead = 0;
        const msize_t   buffLen   = 32;
        byte_t          buff [buffLen];

        std::stringstream ss;

        for (msize_t i = 0; i < pyLen; i += buffLen)
        {
            if ((pyLenRead = packet.PayloadPart(buff, buffLen, i)) > 0)
                ss << std::string((char*)buff, pyLenRead);
        }
        LOG_LINE_GLOBAL("ClientEcho", "PGEXT Packet:", ss.str());
    }
private:

    std::string _name;
};


#endif /* __ORC_MEM_STREAM_REMOTE_SENDER_H__ */

