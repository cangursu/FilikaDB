

#ifndef __EXT_ORC15_MEM_STREAM_H__
#define __EXT_ORC15_MEM_STREAM_H__


#include "orc/OrcFile.hh"
#include "OrcInMemGlobals.h"
#include "LoggerGlobal.h"

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




//static int g___idCounter = 0;



/**
 *    MemIOStream : InputStream & OutputStream base class
 *
 *
 *
 */

class MemIOStream
{
public:
    using byte_t = char;//uint8_t;
};




/**
 *    MemOutputStream: ORC's OutputStream/InputStream interface implementation
 *
 *
 */

template <class U>
class MemStream : public orc::OutputStream,
                  public orc::InputStream,
                  public MemIOStream
{
public:
    MemStream(const std::string &name) : _name(name)
    {
        AddNewBuffer();
    }
    ~MemStream() override
    {
        close();
    }
    uint64_t getLength() const override
    {
        return Len();
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
        if (nullptr == buf) return;

        uint64_t lenWritten = 0;
        for( const byte_t* pbuff = static_cast<const byte_t*>(buf); length > 0; )
        {
            auto &lbuff = _listBuffer[_listSize-1];
            lenWritten = lbuff.Appand(pbuff, length);

            length -= lenWritten;
            pbuff  += lenWritten;

            if (lbuff.Size() <= lbuff.Idx())
                AddNewBuffer();
        }
    }

    virtual void read  (void* buf, uint64_t length, uint64_t offset) override
    {
        if (nullptr == buf) return;

        uint64_t sze = 0;
        uint64_t len = 0;
        uint64_t idx = (uint64_t)((double)offset/(double)g_defBufferSize);
        uint64_t pos = offset % g_defBufferSize;

        U *buffer = static_cast<U*>(buf);

        while (length > 0 && idx < _listSize)
        {
            Buffer<U> &item = _listBuffer[idx];
            sze = item.Idx();
            len   = (pos + length) > sze ? (pos > sze ? 0 : sze - pos) : length;

            std::memcpy(buffer, item.Ptr() + pos, len);

            pos     = 0;
            buffer += len;
            length -= len;
            idx++;
        }
     }


    const std::string& getName() const override
    {
        return _name;
    }

    void close() override
    {
        //_buffer.Clear();
    }

    std::string dump(const std::string &msg = "")
    {
        std::stringstream ss;

        ss << /*GetDate() <<*/ "Dump ---> " << msg << std::endl;
        ss << /*GetDate() <<*/ "Size   :  " << Size() << std::endl;
        ss << /*GetDate() <<*/ "Length :  " << Len()  << std::endl;
        ss << /*GetDate() <<*/ "Dump ---< ";

        return std::move(ss.str());
    }

    uint64_t Size() const { return _listSize * g_defBufferSize; }
    uint64_t Len()  const { return ((_listSize-1) * g_defBufferSize) + _listBuffer[_listSize-1].Idx(); }
    uint64_t Cnt()  const { _listBuffer.size();  }


private :
    std::string         _name;

    template <class T> class Buffer
    {
        public:
            virtual ~Buffer()
            {
                //LOG_LINE_GLOBAL("buffer", " ___id = ", ___id);
                Clear();
            }

            Buffer(orc::MemoryPool &mpool = PGMemPool::Pool(), uint64_t size = g_defBufferSize)
                : _mpool(mpool)
                , _size (size)
            {
                //LOG_LINE_GLOBAL("buffer", "___id = ", ___id);
                _ptr = _mpool.malloc(_size);
            }

            Buffer(const Buffer &other)
                : _mpool (other._mpool)
                , _size  (other._size)
                , _idx   (other._idx)
            {
                //LOG_LINE_GLOBAL("buffer", "___id = ", ___id, " other.___id = ", other.___id);
                _ptr = _mpool.malloc(_size);
                std::memcpy(_ptr, other._ptr, _idx);
            }

            Buffer(Buffer &&other)
                : _mpool (other._mpool)
                , _size  (other._size)
                , _idx   (other._idx)
                , _ptr   (other._ptr)
            {
                //LOG_LINE_GLOBAL("buffer", "&& ___id = ", ___id, " other.___id = ", other.___id);
                other._ptr  = nullptr;
                other.Clear();
            }

            uint64_t Size() const  { return _size;        }
            uint64_t Idx()  const  { return _idx;         }
            T *      Ptr()  const  { return _ptr;         }
            T *      Pos()  const  { return _ptr + _idx;  }

            void Resize(uint64_t addSize)
            {
                uint64_t size = _size;
                if ((_idx + addSize) > size)
                {
                    while ((_idx + addSize) > size)
                        size *= 2;
                    T *ptr  = _ptr;
                    _ptr = (T*) _mpool.malloc(size);
                    memcpy(_ptr, ptr, _idx);
                    _size = size;

                    _mpool.free(ptr);
                }
            }

            void Clear()
            {
                if (_ptr) _mpool.free(_ptr);
                _ptr  = nullptr;
                _size = 0;
                _idx  = 0;
            }

            uint64_t Appand(const byte_t *buff, uint64_t len, bool doResize = false)
            {
                uint64_t  amount = len * sizeof(T);
                if (doResize)
                {
                    Resize(amount);
                }
                else
                {
                    if (_idx + amount > _size)
                        amount = _size - _idx;

                }

                std::memcpy(_ptr + _idx, buff, amount);
                _idx += amount;
                return amount;
            }

        private :

            orc::MemoryPool &_mpool;

            uint64_t         _size = 0;
            uint64_t         _idx  = 0;
            T               *_ptr  = nullptr;

            //int              ___id        = ++g___idCounter;
    };

    size_t AddNewBuffer()
    {
        Buffer<byte_t> buff;
        _listBuffer.push_back(std::move(buff));
        return _listSize++;
    }

    std::deque <Buffer<U> > _listBuffer;
    size_t                  _listSize   = 0;  // prevent to call list::size();
};


#endif  //__EXT_ORC15_MEM_STREAM_H__


