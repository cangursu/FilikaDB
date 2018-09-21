

#ifndef __EXT_ORC15_MEM_STREAM_H__
#define __EXT_ORC15_MEM_STREAM_H__

#include "orc/OrcFile.hh"

#include "MemStream.h"
//#include "MemStreamGlobals.h"
//#include "OrcInMemGlobals.h"
//#include "LoggerGlobal.h"



#include <deque>

/**
 *    PGMemPool : Memory Manager
 *
 *     PGMemPool is an interface implamantatipon to allocate
 *     and release heap from PostgreSQL Server
 *
 */


class PGMemPool : public orc::MemoryPool
{
public:
    virtual ~PGMemPool()  {}
    virtual char* malloc(uint64_t size);
    virtual void  free(char* p);

    static PGMemPool &Pool();
};



template <class U>
class OrcMemStream : public orc::OutputStream
                   , public orc::InputStream
                   , public MemStream<U>
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
        return MemStream<U>::Len();
    }
    uint64_t getNaturalWriteSize() const override
    {
        return /* 128 * */1024;
    }
    virtual uint64_t getNaturalReadSize() const override
    {
        return 1024;
    }
    virtual void write (const void* buf, size_t length) override
    {
        MemStream<U>::Write(buf, length);
    }

    virtual void read  (void* buf, uint64_t length, uint64_t offset) override
    {
        MemStream<U>::Read(buf, length, offset);
    }


    const std::string& getName() const override
    {
        return _name;
    }

    void close() override
    {
        MemStream<U>::close();
    }

private :
    std::string         _name;
};


#endif  //__EXT_ORC15_MEM_STREAM_H__


