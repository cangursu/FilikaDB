

#ifndef __SOCKET_RESULT_H__
#define __SOCKET_RESULT_H__

#include <cstdint>

enum class SocketResult : std::int16_t
{
    //SR_FINISH          =   2,
    SR_TIMEOUT         =   2,
    SR_EMPTY           =   1,
    SR_SUCCESS         =   0,
    SR_ERROR           =  -1,
    SR_ERROR_PRM       =  -2,
    SR_ERROR_EOF       =  -3,
    SR_ERROR_BIND      =  -4,
    SR_ERROR_ACCEPT    =  -5,
    SR_ERROR_CONNECT   =  -6,
    SR_ERROR_SEND      =  -7,
    SR_ERROR_READ      =  -8,
    SR_ERROR_EPOLL     =  -9,
    SR_ERROR_LEN       = -10,
    SR_ERROR_AGAIN     = -11,
    SR_ERROR_REUSEADDR = -12,
    SR_ERROR_SOCKOPT   = -13,
    SR_ERROR_CRC       = -14,
    SR_ERROR_NOTCLIENT = -15,
};



#endif // __SOCKET_RESULT_H__