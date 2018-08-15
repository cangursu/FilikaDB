

#ifndef __SOCKET_RESULT_H__
#define __SOCKET_RESULT_H__

#include <cstdint>

enum class SocketResult : std::int16_t
{
    SR_SUCCESS         =   0,
    SR_ERROR           =  -1,
    SR_FINISH          =  -2,
    SR_ERROR_EOF       =  -3,
    SR_ERROR_BIND      =  -4,
    SR_ERROR_CONNECT   =  -5,
    SR_ERROR_SEND      =  -6,
    SR_ERROR_READ      =  -7,
    SR_ERROR_EPOLL     =  -8,
    SR_ERROR_LEN       =  -9,
    SR_ERROR_AGAIN     = -10,
    SR_ERROR_REUSEADDR = -11,
    SR_ERROR_SOCKOPT   = -12,
};



#endif // __SOCKET_RESULT_H__