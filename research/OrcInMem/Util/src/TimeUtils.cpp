/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TimeUtils.cpp
 * Author: can.gursu
 *
 * Created on 19 October 2018, 15:21
 */

#include "TimeUtils.h"

#define NS_IN_SEC 1000000000l


int nsleep(long nanoseconds)
{
        timespec reqXXX {  .tv_sec = (nanoseconds / 1000000000) , .tv_nsec = nanoseconds - ((nanoseconds / 1000000000) *  1000000000) };

    timespec rem{0,0}, req {.tv_sec = 0, .tv_nsec = nanoseconds};
    nanosleep(&req, &rem);
}


int msleep(long miliseconds)
{
   timespec req, rem;

   if(miliseconds > 999)
   {
        req.tv_sec  = (int)(miliseconds / 1000);                           /* Must be Non-Negative */
        req.tv_nsec = (miliseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }
   else
   {
        req.tv_sec  = 0;                        /* Must be Non-Negative */
        req.tv_nsec = miliseconds * 1000000;    /* Must be in range of 0 to 999999999 */
   }

   return nanosleep(&req , &rem);
}


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
