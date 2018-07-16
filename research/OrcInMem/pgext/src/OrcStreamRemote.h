/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcStreamRemote.h
 * Author: can.gursu
 *
 * Created on 13 July 2018, 10:38
 */

#ifndef __ORC_STREAM_REMOTE_H__
#define __ORC_STREAM_REMOTE_H__

#include "orc/OrcFile.hh"

template <class U>
class OrcMemStream : public orc::OutputStream
                   , public orc::InputStream
{
public:
    OrcMemStream(const std::string &name) : _name(name)
    {
    }

    ~OrcMemStream() override
    {
        close();
    }

    uint64_t getLength() const override
    {
    }

    uint64_t getNaturalWriteSize() const override
    {
        return /*128 * */1024;
    }

    virtual uint64_t getNaturalReadSize() const override
    {
        return 1024;
    }

    virtual void write (const void* buf, size_t length) override
    {
    }

    virtual void read  (void* buf, uint64_t length, uint64_t offset) override
    {
    }

    const std::string& getName() const override
    {
        return _name;
    }

    void close() override
    {
    }

private :
    std::string         _name;
};



#endif // __ORC_STREAM_REMOTE_H__

