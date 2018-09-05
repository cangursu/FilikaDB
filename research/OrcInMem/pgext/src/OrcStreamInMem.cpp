
#ifdef __cplusplus
extern "C" {
#endif

#include "postgres.h"
//#include "utils/builtins.h"

#ifdef __cplusplus
}
#endif



#include "OrcStreamInMem.h"
#include "LoggerGlobal.h"



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
    //LOG_LINE_GLOBAL("InMemTest", "size:", size, ", p:", (int)p);
    return static_cast<char*>(p);
}

void  PGMemPool::free(char* p)
{
    //LOG_LINE_GLOBAL("InMemTest", (int)p);
    if (p) pfree(p);
}


