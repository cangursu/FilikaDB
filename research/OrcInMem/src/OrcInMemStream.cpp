


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
    return static_cast<char*>(palloc(size)); 
}

void  PGMemPool::free(char* p)
{
    if (p) pfree(p);
}






/**
 *    MemOutputStream: ORC's OutputStream interface implementation 
 *
 */


void MemOutputStream::write(const void* buf, size_t length) 
{
	// LOG_LINE_GLOBAL("OutputStream", "length = %d", length);    
    _buffer.Appand((byte_t*)buf, length);
    _curIdx = _buffer.Idx();
	// LOG_LINE_GLOBAL("OutputStream", "Idx = %d", _buffer.Idx());
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

