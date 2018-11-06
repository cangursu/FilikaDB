

#ifndef __MEM_STREAM_H__
#define __MEM_STREAM_H__


#include "MemStreamGlobals.h"

#include <cstring>
#include <string>
#include <sstream>
#include <deque>
#include <vector>
#include <iostream>
//#include <iomanip>



/**
 *    MemoryPool: Manage memory allocation and deallocation
 *
 */

class MemoryPool
{
    public:
        virtual char* malloc(std::uint64_t size) = 0;
        virtual void  free  (char* p)            = 0;
};

class MemoryPoolDefault : public MemoryPool
{
    public:
        virtual char* malloc(std::uint64_t size)  { return new char[size];  }
        virtual void  free  (char* p)             { delete [] p;            }
};


extern MemoryPoolDefault g_poolDefault;


/**
 *    MemOutputStream: ORC's OutputStream/InputStream interface implementation
 *
 */


template <class U>
class MemStream
{
public:
    using byte_t = uint8_t;

public:
    MemStream()
    {
        AddNewBuffer();
    }


    MemStream(const MemStream &other) = default;
    MemStream(MemStream &&other)      = default;

    MemStream(const std::vector<byte_t> &buf) : MemStream() { Write(buf); }
    MemStream(const void* buf, size_t length) : MemStream() { Write(buf, length); }



    virtual ~MemStream()
    {
        close();
    }

    void Write (const std::vector<byte_t> &buf)
    {
        Write(&buf[0], buf.size());
    }

    void Write (const void* buf, size_t length)
    {
        if (nullptr == buf) return;

        std::uint64_t lenWritten = 0;
        for( const byte_t* pbuff = static_cast<const byte_t*>(buf); length > 0; )
        {
            auto &lbuff = _listBuffer[_listSize-1];
            lenWritten = lbuff.Appand(pbuff, length);

            length -= lenWritten;
            pbuff  += lenWritten;

            if (lbuff.Size() <= lbuff.Idx())
            {
                AddNewBuffer();
            }
        }
    }

    friend inline bool operator == (const MemStream& left, const MemStream& right)
    {
        bool res = false;
        if (left.Len() == right.Len())
        {
            res = true;

            size_t cnt = std::max(left.Cnt(), right.Cnt());
            for (size_t idx = 0; idx < cnt && res == true; ++idx)
            {
                res = (left._listBuffer[idx] == right._listBuffer[idx]);
            }
        }
        return res;
    }

    std::uint64_t Read (void* buf, std::uint64_t length, std::uint64_t offset = 0) const
    {
        if (nullptr == buf) return 0L;

        std::uint64_t readed = 0;

        std::uint64_t sze = 0;
        std::uint64_t len = 0;
        std::uint64_t idx = (std::uint64_t)((double)offset/(double)g_defBufferSize);
        std::uint64_t pos = offset % g_defBufferSize;

        U *buffer = static_cast<U*>(buf);

        while (length > 0 && idx < _listSize)
        {
            const Buffer<U> &item = _listBuffer[idx];
            sze = item.Idx();
            len   = (pos + length) > sze ? (pos > sze ? 0 : sze - pos) : length;

            std::memcpy(buffer, item.Ptr() + pos, len);  //TODO: std::memcpy should call from MemPool

            pos     = 0;
            readed += len;
            buffer += len;
            length -= len;
            idx++;
        }
        return readed;
     }

    void reset()
    {
        close();
        AddNewBuffer();
    }

    void close()
    {
        _listBuffer.clear();
        _listSize = 0;
    }

    std::string Dump(const std::string &msg = "") const
    {
        std::stringstream ss;

        ss << "Dump Stream ---> " << msg    << std::endl;
        ss << "Size   :  " << Size() << std::endl;
        ss << "Length :  " << Len()  << std::endl;
        ss << "Data   :  \n";
        {
            const int line = 16;
            const int buffTmpSize = 64;

            long newLinePos   = ss.tellp();
            long newLineCount = 1;

            uint8_t buffRead[line];
            char    buffTmp[buffTmpSize];
            for (std::uint64_t offset = 0, len = Len(); offset < len; offset += line)
            {
                memset(buffRead, 0, line);
                Read(buffRead, line, offset);


                // ......................................................
                // Unable to include <iomanip>  ... have to use snprintf :(
                // ......................................................
                int j = snprintf(buffTmp, buffTmpSize, "%08d - ", offset);
                for (int i = 0; (i < 16) && (i + offset < len); ++i)
                    j += snprintf(buffTmp + j, buffTmpSize, "%02x ", (int)buffRead[i]);
                ss << buffTmp;
// ::::::::::::::::::::::::::::
//              ss << std::dec << std::setfill(' ') << std::setw(8) << offset << " - ";
//              for (int i = 0; (i < 16) && (i + offset < len); ++i)
//                  ss << std::hex << std::setfill('0') << std::setw(2) << (int)buff[i] << " ";
// ::::::::::::::::::::::::::::
                long nlp = ss.tellp();
                for(long i = nlp - newLinePos; i < 64; ++i)
                    ss << " ";

                for (int i = 0; (i < 16) && (i + offset < len); ++i)
                {
                    switch (buffRead[i])
                    {
                        case '\0' : ss << "NL"; break;
                        case '\n' : ss << "LF"; break;
                        case '\t' : ss << "TB"; break;
                        case '\v' : ss << "TB"; break;
                        case '\b' : ss << "BS"; break;
                        case '\r' : ss << "CR"; break;
                        default   : snprintf(buffTmp, buffTmpSize, "%1c", buffRead[i]); ss << buffTmp;
                                    //ss << /* :::::::::::::::::::::  std::setw(1) << ::::::::::::::::::::::::::: */ buffRead[i];
                    }
                    ss << " ";
                }
                ss << std::endl;

                newLinePos = ss.tellp();
                newLineCount++;
            }
        }

        ss << /*GetDate() <<*/ "Dump ---<\n";

        return std::move(ss.str());
    }

    std::uint64_t Size() const { return _listSize * g_defBufferSize; }
    std::uint64_t Len()  const { return ((_listSize-1) * g_defBufferSize) + _listBuffer[_listSize-1].Idx(); }
    std::uint64_t Cnt()  const { return _listSize; }//return _listBuffer.size();  }


    friend inline bool operator!=(const MemStream& left, const MemStream& right){ return !(left == right); }

private :

    template <class T> class Buffer
    {
        public:
            virtual ~Buffer()
            {
                //LOG_LINE_GLOBAL("buffer", " ___id = ", ___id);
                Clear();
            }

            Buffer(MemoryPool &mpool = g_poolDefault, std::uint64_t size = g_defBufferSize)
                : _mpool(mpool)
                , _size (size)
            {
                //LOG_LINE_GLOBAL("buffer", "___id = ", ___id);
                _ptr = (T*)_mpool.malloc(_size);
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

            std::uint64_t Size() const  { return _size;        }
            std::uint64_t Idx()  const  { return _idx;         }
            T *           Ptr()  const  { return _ptr;         }
            T *           Pos()  const  { return _ptr + _idx;  }

            void Resize(std::uint64_t addSize)
            {
                std::uint64_t size = _size;
                if ((_idx + addSize) > size)
                {
                    while ((_idx + addSize) > size)
                        size *= 2;
                    T *ptr  = _ptr;
                    _ptr = (T*) _mpool.malloc(size);
                    memcpy(_ptr, ptr, _idx);
                    _size = size;

                    _mpool.free((char *)ptr);
                }
            }

            void Clear()
            {
                if (_ptr) _mpool.free(reinterpret_cast<char *>(_ptr));
                _ptr  = nullptr;
                _size = 0;
                _idx  = 0;
            }

            std::uint64_t Appand(const byte_t *buff, std::uint64_t len, bool doResize = false)
            {
                std::uint64_t  amount = len * sizeof(T);
                if (doResize)
                {
                    Resize(amount);
                }
                else
                {
                    if (_idx + amount > _size) amount = _size - _idx;
                }

                std::memcpy(_ptr + _idx, buff, amount);
                _idx += amount;
                return amount;
            }

            friend inline bool operator == (const Buffer& left, const Buffer& right)
            {
                return (left._idx == right._idx) && (0 == std::memcmp(left._ptr, right._ptr, left._idx * sizeof(T)));
            }

            friend inline bool operator!=(const Buffer& left, const Buffer& right)
            {
                return !(left == right);
            }

        private :

            MemoryPool      &_mpool;

            std::uint64_t    _size = 0;
            std::uint64_t    _idx  = 0;
            T               *_ptr  = nullptr;
    };

    size_t AddNewBuffer()
    {
        Buffer<U> buff;
        _listBuffer.push_back(std::move(buff));
        return _listSize++;
    }

    std::deque <Buffer<U> > _listBuffer;
    size_t                  _listSize   = 0;  // prevent to call list::size();
};


#endif  //__MEM_STREAM_H__


