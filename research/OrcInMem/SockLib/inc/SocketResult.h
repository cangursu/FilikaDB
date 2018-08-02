

#ifndef __SOCKET_RESULT_H__
#define __SOCKET_RESULT_H__

#include <cstdint>

enum class SocketResult : std::int16_t
{
    SR_SUCCESS         =  0,
    SR_ERROR           = -1,
    SR_ERROR_BIND      = -2,
    SR_ERROR_CONNECT   = -3,
    SR_ERROR_SEND      = -4,
    SR_ERROR_READ      = -5,
    SR_ERROR_EPOLL     = -6,
    SR_ERROR_LEN       = -7,
    SR_ERROR_AGAIN     = -8,
    SR_ERROR_REUSEADDR = -9,
};



#endif // __SOCKET_RESULT_H__