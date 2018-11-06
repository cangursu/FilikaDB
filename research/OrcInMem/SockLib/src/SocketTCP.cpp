

#include "SocketTCP.h"
#include "GeneralUtils.h"

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <netdb.h>
 #include <arpa/inet.h>


SocketTCP::SocketTCP(const char *name /*= "NA"*/)
    : Socket(name)
{
}
SocketTCP::SocketTCP(int fd, const char *name)
    : Socket(fd, name)
{
}

/*
SocketTCP::SocketTCP(SocketTCP &&s)
    : Socket(std::move(s))
{
}
*/

SocketTCP::SocketTCP(const std::string &address, std::uint16_t port, const char *name)
    : Socket(name)
    , _address(address)
    , _port(port)
{
}

SocketTCP::~SocketTCP()
{
}

/*
SocketTCP& SocketTCP::operator=(SocketTCP &&s)
{
    Socket::operator = (std::move(s));
    return *this;
}
*/

SocketResult SocketTCP::Init()
{
    return Socket::Init(AF_INET, SOCK_STREAM);
}

SocketResult SocketTCP::InitServer()
{
    SocketResult res = Socket::Init(AF_INET, SOCK_STREAM);

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

SocketResult SocketTCP::Connect()
{
    sockaddr_in addr {
        .sin_family = AF_INET,
        .sin_port   = htons(_port),
    };
    inet_pton(AF_INET, _address.c_str(), &addr.sin_addr);

    SocketResult res = SocketResult::SR_ERROR_CONNECT;
    char node[NI_MAXHOST];
    if (0 == getnameinfo((struct sockaddr*)&addr, sizeof(addr),  node, NI_MAXHOST, NULL, 0, 0))
    {
        errno = 0;
        res = (::connect(fd(), (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) ?
            SocketResult::SR_ERROR_CONNECT : SocketResult::SR_SUCCESS;
    }

    return res;
}

/*
SocketResult SocketTCP::Connect()
{
    SocketResult res = SocketResult::SR_ERROR;

    hostent *host = gethostbyname(_address.c_str());
    if (host)
    {
        sockaddr_in addr {  .sin_family = AF_INET,
                            .sin_port   = htons(_port),
                            .sin_addr   = * ((struct in_addr *)(host->h_addr))
                     };
        std::memset(&(addr.sin_zero), 0, 8);

        errno = 0;
        res = (::connect(fd(), (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) ?
            SocketResult::SR_ERROR_CONNECT : SocketResult::SR_SUCCESS;
    }

    return res;
}
*/
SocketResult SocketTCP::SetNonBlock()
{
    SocketResult res = SocketResult::SR_ERROR;

    int flags = fcntl(fd(), F_GETFL, 0);
    if (-1 == flags)
    {
        std::cerr << "ERROR: fcntl\n";
        return SocketResult::SR_ERROR;
    }

    flags |= O_NONBLOCK;
    if (-1 == fcntl(fd(), F_SETFL, flags))
    {
        std::cerr << "ERROR: fcntl\n";
        return SocketResult::SR_ERROR;
    }

    return SocketResult::SR_SUCCESS;
}

ssize_t SocketTCP::Read  (void *pdata, size_t len)
{
    //std::cout << "SocketTCP(" << Name() << ")::Read ->   fd:"<< fd() << " - len:" << len << std::endl;
    ssize_t bytes = ::read(fd(), pdata, len);
    //std::cout << "SocketTCP(" << Name() << ")::Read -<   fd:"<< fd() << " - bytes:" << bytes << std::endl;

    if (bytes < 0)
    {
        //std::cerr << "SocketTCP(" << Name() << ")::Read  ERROR - " << ErrnoText(errno) <<  "(" << errno << ") " << strerror(errno) << std::endl;
        switch(errno)
        {
            case EAGAIN :
//            case EWOULDBLOCK :
//                std::cerr << "SocketTCP(" << Name() << ")::Read  ERROR - " << ErrnoText(errno) <<  "(" << errno << ") " << strerror(errno) << std::endl;
                break;
            default :
                std::cerr << "SocketTCP(" << Name() << ")::Read  ERROR - " << ErrnoText(errno) <<  "(" << errno << ") " << strerror(errno) << std::endl;
                Release();
        }
    }
    else if (bytes == 0)
    {
        //std::cerr << "SocketTCP(" << Name() << ")::Read  Peer disconnected\n";
        Release();
    }

    return bytes;
}


ssize_t SocketTCP::Write (const void *pdata, size_t len)
{
    //std::cout << "SocketTCP(" << Name() << ")::Write ->   fd:"<< fd() << " - len:" << len << std::endl;

    ssize_t bytes = -1;
    if (false == IsGood())
        errno = EBADF;
    else
        bytes = ::write(fd(), pdata, len);


    if (bytes == -1)
    {
        std::cerr << "SocketTCP(" << Name() << ")::Write  ERROR - (" << errno << "-" << ErrnoText(errno) << ") " << strerror(errno) << std::endl;
        Release();
    }
    else if (bytes == 0)
    {
        //std::cerr << "SocketTCP(" << Name() << ")::Write  Peer disconnected\n";
        Release();
    }
    return bytes;
}







