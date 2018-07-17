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

#include "orc/OrcFile.hh"


class OrcMemStreamRemoteSender
    : public orc::OutputStream
    , public orc::InputStream
{
public:
    OrcMemStreamRemoteSender();
    OrcMemStreamRemoteSender(const OrcMemStreamRemoteSender& orig);
    virtual ~OrcMemStreamRemoteSender();

    virtual uint64_t    getLength() const;
    virtual uint64_t    getNaturalReadSize() const;
    virtual uint64_t    getNaturalWriteSize() const;
    virtual void        read(void* buf, uint64_t length, uint64_t offset);
    virtual void        write(const void* buf, size_t length);
    virtual const       std::string& getName() const;
    virtual void        close();
private:

};

#endif /* __ORC_MEM_STREAM_REMOTE_SENDER_H__ */

