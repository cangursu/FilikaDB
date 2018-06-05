

#include "ext_orc15_time.h"


#define NS_IN_SEC 1000000000l


timespec operator + (const timespec &a, const timespec &b) 
{
    long nsecs = (a.tv_nsec % NS_IN_SEC) + (b.tv_nsec % NS_IN_SEC);
    return timespec 
    {
        a.tv_sec + b.tv_sec + (a.tv_nsec / NS_IN_SEC) + (b.tv_nsec / NS_IN_SEC) + (nsecs / NS_IN_SEC),
        nsecs % NS_IN_SEC
    };
}

timespec operator - (const timespec &a, const timespec& b)
{
    if(a.tv_nsec < b.tv_nsec)
    {
        return timespec 
        {
            a.tv_sec - b.tv_sec - 1,
            NS_IN_SEC + a.tv_nsec - b.tv_nsec,
        };
    }
    else
    {
        return  timespec
        {
            a.tv_sec  - b.tv_sec,
            a.tv_nsec - b.tv_nsec,
        };
    }
}

