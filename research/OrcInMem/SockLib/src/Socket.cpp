

#include "Socket.h"
#include "SocketUtils.h"

#include <fcntl.h>
#include <unistd.h>


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

Socket::~Socket()
{
    Release();
}

Socket& Socket::operator=(Socket &&s)
{
    _sock = s._sock;
    _name = s._name;

    s._sock = -1;
    return *this;
}

SocketResult Socket::Init ()
{
    Release();

    SocketResult res = SocketResult::SR_SUCCESS;
    if ((_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "ERROR : Socket\n";
        res = SocketResult::SR_ERROR;
    }

    return res;
}

SocketResult Socket::InitServer()
{
    SocketResult res = Init();

    if (SocketResult::SR_SUCCESS == res)
    {
        int opt = 1;
        if (setsockopt(fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
        {
            //std::cerr << "Setsockopt\n";
            res = SocketResult::SR_ERROR_REUSEADDR;
        }
        else
        {
            sockaddr_in server_addr {.sin_family = AF_INET,
                                     .sin_port   = htons(_port) };
            server_addr.sin_addr.s_addr = INADDR_ANY;
            std::memset(&(server_addr.sin_zero), 0, 8);
            if (bind(fd(), (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
                res = SocketResult::SR_ERROR_BIND;
        }
    }

    return res;
}


void Socket::Release()
{
    SocketResult res = SocketResult::SR_SUCCESS;
    if (-1 != _sock)
    {
        if (-1 == ::close(_sock)) res = SocketResult::SR_ERROR;
        _sock = -1;
    }
}

SocketResult Socket::Connect()
{
    SocketResult res = SocketResult::SR_ERROR;

    hostent *host = gethostbyname(_address.c_str());
    if (host)
    {
        sockaddr_in addr {  .sin_family = AF_INET,
                            .sin_port   = htons(_port),
                            .sin_addr   = *((struct in_addr *)(host->h_addr))
                     };
                     std::memset(&(addr.sin_zero), 0, 8);

    res = (::connect(fd(), (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) ?
        SocketResult::SR_ERROR_CONNECT : SocketResult::SR_SUCCESS;
    }

    return res;
}

SocketResult Socket::SetNonBlock()
{
    SocketResult res = SocketResult::SR_ERROR;

    int flags = fcntl(_sock, F_GETFL, 0);
    if (-1 == flags)
    {
        perror("fcntl");
        return SocketResult::SR_ERROR;
    }

    flags |= O_NONBLOCK;
    if (-1 == fcntl(_sock, F_SETFL, flags))
    {
        perror("fcntl");
        return SocketResult::SR_ERROR;
    }

    return SocketResult::SR_SUCCESS;
}








