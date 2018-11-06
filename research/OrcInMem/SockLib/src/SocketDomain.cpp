/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SocketDomain.cpp
 * Author: can.gursu
 *
 * Created on 13 July 2018, 09:16
 */

#include "SocketDomain.h"
#include "GeneralUtils.h"

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>



SocketDomain::SocketDomain(const char *name /*= "NA"*/)
        : Socket(name)
{
}

SocketDomain::SocketDomain(int fd, const char *name /*= "NA"*/)
    : Socket(fd, name)
{
}

SocketDomain::SocketDomain(SocketDomain &&val)
{
    operator = (std::move(val));
}


SocketDomain::~SocketDomain()
{
    Release();
}

SocketDomain& SocketDomain::operator=(SocketDomain &&s)
{
    SocketPath(s._path.c_str());
    Socket::operator = (std::move(s));
    return *this;
}

SocketResult SocketDomain::Init()
{
    return Socket::Init(AF_UNIX, SOCK_STREAM);
}

SocketResult SocketDomain::Init(const char *path)
{
    SocketPath(path);
    return Init();
}

SocketResult SocketDomain::InitServer(const char *path /*= nullptr*/)
{
    //std::cout << "SocketDomain(" << Name() << ")InitServer sock:" << fd() << std::endl;
    SocketResult res = SocketResult::SR_ERROR;

    if (SocketResult::SR_SUCCESS == Init())
    {
        //_addr.sun_family = AF_UNIX;

        sockaddr_un addr { .sun_family = AF_UNIX };
        if (path)  _path = path;
        std::strcpy(addr.sun_path, _path.c_str());

        unlink(_path.c_str());
        if (0 == ::bind(fd(), (struct sockaddr *) &addr, sizeof (addr)))
        {
            if (0 == ::listen(fd(), 150))
                res = SocketResult::SR_SUCCESS;
        }
    }

    return res;
}

SocketResult SocketDomain::Connect  ()
{
    sockaddr_un addr = {};
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    std::strcpy(addr.sun_path, _path.c_str());
    return (::connect(fd(), (struct sockaddr*) &addr, sizeof (sockaddr_un)) == 0) ? SocketResult::SR_SUCCESS : SocketResult::SR_ERROR_CONNECT;
}

SocketDomain SocketDomain::Accept   ()
{
    return std::move(SocketDomain(::accept(fd(), NULL, NULL), "ClientDomain"));
}

int SocketDomain::AcceptFd ()
{
    return ::accept(fd(), NULL, NULL);
}

SocketResult SocketDomain::SetNonBlock()
{
    SocketResult res = SocketResult::SR_ERROR;

    int flags = fcntl(fd(), F_GETFL, 0);
    if (-1 == flags)
    {
        std::cerr << "ERROR : fcntl GETFL\n";
        return SocketResult::SR_ERROR;
    }

    flags |= O_NONBLOCK;
    if (-1 == fcntl(fd(), F_SETFL, flags))
    {
        std::cerr << "ERROR : fcntl SETFL\n";
        return SocketResult::SR_ERROR;
    }

    return SocketResult::SR_SUCCESS;
}

ssize_t SocketDomain::Read(void *pdata, size_t lenData)
{
    //std::cout << "SocketDomain(" << Name() << ")::Read --->  sock:" << fd() << std::endl;

    ssize_t bytes = ::read(fd(), pdata, lenData);
    if (bytes < 0)
    {
        switch(errno)
        {
            case EAGAIN :
//            case EWOULDBLOCK :
//                std::cerr << "SocketDomain(" << Name() << ")::Read  ERROR - " << ErrnoText(errno) <<  "(" << errno << ") " << strerror(errno) << std::endl;
                break;
            default :
                std::cerr << "SocketDomain(" << Name() << ")::Read  ERROR - "  << ErrnoText(errno) <<  "(" << errno << ") " << strerror(errno) << std::endl;
                Release();
        }

    }

    if (bytes == 0)
    {
        //std::cerr << "SocketDomain(" << Name() << ")::Read  Peer disconnected\n";
        Release();
    }

    return bytes;
}

ssize_t  SocketDomain::Write(const void *pdata, size_t len)
{
    //std::cout << "SocketDomain(" << Name() << ")::Write --->  sock:" << fd() << ", len:" << len << std::endl;
    ssize_t bytes = ::write(fd(), pdata, len);
    //std::cout << "SocketDomain(" << Name() << ")::Write ---<  bytes:" << bytes << std::endl;

    if (bytes == -1)
    {
        std::cerr << "SocketDomain(" << Name() << ")::Write  ERROR - " << errno << " (" << ErrnoText(errno) << ") " << strerror(errno) << std::endl;
        Release();
    }
    if (bytes == 0)
    {
        //std::cerr << "SocketDomain(" << Name() << ")::Write  Peer disconnected\n";
        Release();
    }

    return bytes;
}
