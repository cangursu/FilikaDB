

#include "Socket.h"

#include <sys/socket.h>
#include <unistd.h>


Socket::~Socket()
{
    Release();
}

Socket::Socket(const char *name /*= "NA"*/)
    : _name (name)
{
}
Socket::Socket(int s, const char *name)
    : _sock (s)
    , _name (name)
{
}

Socket::Socket(Socket &&s)
    : _sock (s._sock)
    , _name (s._name)
{
    s._sock = -1;
}

Socket& Socket::operator=(Socket &&s)
{
    _sock = s._sock;
    _name = s._name;

    s._sock = -1;
    return *this;
}

SocketResult Socket::Init (int domain, int type)
{
    Release();
    return ((_sock = ::socket(domain, type, 0)) == -1) ? SocketResult::SR_ERROR : SocketResult::SR_SUCCESS;
}

SocketResult Socket::Release()
{
    SocketResult res = SocketResult::SR_SUCCESS;
    if (-1 != _sock)
    {
        if (-1 == ::close(_sock)) res = SocketResult::SR_ERROR;
        _sock = -1;
    }
}

