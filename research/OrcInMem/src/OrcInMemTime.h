
#ifndef __EXT_ORC15_TIME_H_
#define __EXT_ORC15_TIME_H__

#include <time.h>


timespec operator + (const timespec &a, const timespec &b);
timespec operator - (const timespec &a, const timespec& b);

#define TS_TO_NANO(ts) ((ts).tv_sec * 1000000000.0 + (ts).tv_nsec)
#define TS_TO_MICRO(ts)((ts).tv_sec * 1000000.0    + (ts).tv_nsec / 1000.0)
#define TS_TO_MILI(ts)
#define TS_TO_SEC(ts)  ((ts).tv_sec)



class CallMeasure
{
public:
    CallMeasure() : elapses(0.0) , elapsesCPU(0.0)
    {
    }
    CallMeasure(double e, double ec) : elapses(e) , elapsesCPU(ec)
    {
    }
    CallMeasure(const CallMeasure &other) : elapses(other.elapses) , elapsesCPU(other.elapsesCPU)
    {
    }
    /*
    CallMeasure(CallMeasure &&other)
        : elapses(other.elapses)
        , elapsesCPU(other.elapsesCPU)
    {
    }
    */


    CallMeasure operator + (const CallMeasure &other) const
    {
        return { elapses  + other.elapses, elapsesCPU  + other.elapsesCPU };
    }
    CallMeasure operator - (const CallMeasure &other) const
    {
        return { elapses  - other.elapses, elapsesCPU  - other.elapsesCPU };
    }
    CallMeasure operator += (const CallMeasure &other)
    {
        return { elapses += other.elapses, elapsesCPU += other.elapsesCPU };
    }
    CallMeasure operator -= (const CallMeasure &other)
    {
        return { elapses -= other.elapses, elapsesCPU -= other.elapsesCPU };
    }

//private :
    double elapses    = 0.0;
    double elapsesCPU = 0.0;
};


template <typename Functor>
CallMeasure CallMeasureFrame(Functor f)
{
    timespec ts_start;
    timespec ts_start_cpu;
    timespec ts_end;
    timespec ts_end_cpu;

    clock_gettime(CLOCK_MONOTONIC         , &ts_start     );
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start_cpu );

    f();

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end_cpu   );
    clock_gettime(CLOCK_MONOTONIC         , &ts_end       );

    return { TS_TO_MICRO(ts_end - ts_start), TS_TO_MICRO(ts_end_cpu - ts_start_cpu)} ;
}





#define CALL_MEASURE_FRAME(s1, s2, ...)                                                                           \
    {                                                                                                             \
        struct timeval t_start, t_end;                                                                            \
        gettimeofday(&t_start, nullptr);                                                                          \
        clock_t c_start = clock();                                                                                \
                                                                                                                  \
                                                                                                                  \
        LOG_LINE_GLOBAL(s1, s2);                                                                                  \
         #__VA_ARGS__ ;                                                                                           \
                                                                                                                  \
                                                                                                                  \
        totalCPUTime += (clock() - c_start);                                                                      \
        gettimeofday(&t_end, nullptr);                                                                            \
        totalElapsedTime += (    static_cast<double>(t_end.tv_sec  - t_start.tv_sec ) * 1000000.0                 \
                               + static_cast<double>(t_end.tv_usec - t_start.tv_usec)              ) / 1000000.0; \
    }                                                                                                             \



#endif //__EXT_ORC15_TIME_H_