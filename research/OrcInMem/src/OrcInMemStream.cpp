


#ifdef __cplusplus
extern "C" {
#endif

#include "postgres.h"
#include "utils/builtins.h"

#ifdef __cplusplus
}
#endif




#include "OrcInMemStream.h"
#include "LoggerGlobal.h"

#include <sstream>





/**
 *    PGMemPool : Memory Manager
 *
 */

PGMemPool &PGMemPool::Pool()
{
    static PGMemPool s_pool;
    return s_pool;
}

char* PGMemPool::malloc(uint64_t size)
{
    void *p = nullptr;
    if (size > 0 && (nullptr == (p = palloc(size))))
    {
        LOG_LINE_GLOBAL("***ERROR***", "Memory ERROR... palloc() fialed (size:", size, ")");
    }
    //LOG_LINE_GLOBAL("Mem", "size:", size, ", p:", (int)p);
    return static_cast<char*>(p);
}

void  PGMemPool::free(char* p)
{
    //LOG_LINE_GLOBAL("Mem", (int)p);
    if (p) pfree(p);
}






/**
 *    MemOutputStream: ORC's OutputStream interface implementation
 *
 */


void MemOutputStream::write(const void* buf, size_t length)
{
//    LOG_LINE_GLOBAL("OutputStream-1", "Size = ", Size(), ", Idx = ", Idx(), ", length = %ld", length);

    _buffer.Appand((byte_t*)buf, length);

//    LOG_LINE_GLOBAL("OutputStream-2", "Size = ", Size(), ", Idx = ", Idx(), ", length = %ld", length);
}


std::string MemOutputStream::dump(const std::string &msg/* = "" */)
{
    std::stringstream ss;

    ss << /*GetDate() <<*/ "Dump ---> " << msg << std::endl;
    ss << /*GetDate() <<*/ "Size   :  " << _buffer.Size() << std::endl;
    ss << /*GetDate() <<*/ "Length :  " << _buffer.Idx()  << std::endl;
    ss << /*GetDate() <<*/ "Dump ---< ";

    return std::move(ss.str());
}

