

#ifndef __EXT_ORC15_MEM_STREAM_H__
#define __EXT_ORC15_MEM_STREAM_H__


#include "orc/OrcFile.hh"


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
    virtual void free(char* p);
    
    static PGMemPool &Pool();
};





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
 *    MemOutputStream: ORC's OutputStream interface implementation 
 *
 * 
 */

class MemOutputStream : public orc::OutputStream,
                        public MemIOStream
{
public:
    MemOutputStream(const std::string &name) : _name(name)
    {
    }
    ~MemOutputStream() override
    {
        close();
    }
    uint64_t getLength() const override
    {
        return _curIdx;
    }
    uint64_t getNaturalWriteSize() const override
    {
        return 128 * 1024;
    }
    
    void write(const void* buf, size_t length) override;

    const std::string& getName() const override
    {
        return _name;
    }

    void close() override
    {
        //_curIdx = 0;
        //_chunck.clear();
        //_buffer.Clear();
    }
    
    std::string dump(const std::string &msg = "");
    
    uint64_t Size() const  { return _buffer.Size();  }
    uint64_t Idx()  const  { return _buffer.Idx() ;  }
    byte_t * Ptr()  const  { return _buffer.Ptr() ;  }
    byte_t * Pos()  const  { return _buffer.Pos() ;  }

    
private :
    uint64_t            _curIdx = 0;
    std::string         _name;
    

    template <class T> class Buffer
    {
        public:
            virtual ~Buffer()
            {
                _ptr = _mpool.malloc(_size);
            }
            
            Buffer(orc::MemoryPool &mpool = PGMemPool::Pool())
                : _mpool(mpool)
                , _size (1024)
            {
                _ptr = _mpool.malloc(_size);
            }
                
            Buffer(const Buffer &other)
                : _mpool (other._mpool)
                , _size  (other._size)
            {
                _ptr = _mpool.malloc(_size);
                std::memcpy(_ptr, other._ptr, _size);
            }
            
            Buffer(Buffer &&other)
                : _mpool (other._mpool)
                , _size  (other._size)
                , _ptr   (other._ptr)
            {
                other.Clear();
            }

            uint64_t Size() const  { return _size;        }
            uint64_t Idx()  const  { return _idx;         }
            T *      Ptr()  const  { return _ptr;         }
            T *      Pos()  const  { return _ptr + _idx;  }
            
            void Resize()
            {
                uint64_t    size = _size * 2;
                T          *ptr  = _ptr;
                
                _ptr = (T*) _mpool.malloc(size);
                memcpy(_ptr, ptr, _size);
                _size = size;
                
                _mpool.free(ptr);
            }

            void Clear()
            {
                if (_ptr) _mpool.free(_ptr);
                _ptr = nullptr;
                _size = 0;
                _idx = 0;
            }

            uint64_t Appand(byte_t *buff, uint64_t len)
            {
                uint64_t  amount = len * sizeof(T);
                if (_idx + len > _size)
                    Resize();
                memcpy(_ptr + _idx, buff, amount);
                _idx += amount;
                return amount;
            }

        
        private : 
        
            orc::MemoryPool &_mpool;

            uint64_t         _size = 0;
            uint64_t         _idx  = 0;
            T               *_ptr  = nullptr;            
    };
    
    
    
    
    
    Buffer<byte_t>      _buffer;

};






class MemInputStream : public orc::InputStream,
                       public MemIOStream
{
public:
    MemInputStream(const std::string &name, byte_t *buff, uint64_t len) 
        : _name(name)
        , _ptr(buff)
        , _len(len)
    {
    }
    
    ~MemInputStream() override
    {
    }
    
    virtual uint64_t getLength() const override
    {
        return _len;
    }
    
    virtual uint64_t getNaturalReadSize() const override
    {
        return 1024;
    }
    
    virtual void read(void* buf, uint64_t length, uint64_t offset) override
    {
        memcpy(buf, _ptr + offset, length);
    }
    
    virtual const std::string& getName() const override
    {
        return _name;
    }

    
private :
    std::string  _name;
    byte_t      *_ptr = nullptr;
    uint64_t     _len = 0;
};





#endif  //__EXT_ORC15_MEM_STREAM_H__


