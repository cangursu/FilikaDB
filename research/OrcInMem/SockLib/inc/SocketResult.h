

#ifndef __SOCKET_RESULT_H__
#define __SOCKET_RESULT_H__

#include <cstdint>

enum class SocketResult : std::int16_t
{
    SUCCESS       =  0,
    ERROR         = -1,
    ERROR_BIND    = -2,
    ERROR_CONNECT = -3,
    ERROR_SEND    = -4,
    ERROR_READ    = -5,
    ERROR_EPOLL   = -6,
    ERROR_LEN     = -7,
};



#endif // __SOCKET_RESULT_H__