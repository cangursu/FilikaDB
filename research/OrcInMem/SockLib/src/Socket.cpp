

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

    for (int ntry = 0; (false == IsGood()) && (ntry < 3); ++ntry)
    {
        _sock = ::socket(domain, type, 0);
    }

    return IsGood() ? SocketResult::SR_SUCCESS : SocketResult::SR_ERROR;
}

SocketResult Socket::Release()
{
    SocketResult res = SocketResult::SR_SUCCESS;
    if (-1 != _sock)
    {
        if (-1 == ::close(_sock)) res = SocketResult::SR_ERROR;
        _sock = -1;
    }
    return res;
}

