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
#include "SocketResult.h"

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>




SocketDomain::SocketDomain(const char *name /*= "NA"*/)
        : _name(name)
{
    //std::cout << "SocketDomain(" << _name << ")::SocketDomain sock:" << _sock << std::endl;
    _addr.sun_family = AF_UNIX;
//    strcpy(_addr.sun_path, path);
}

SocketDomain::SocketDomain(int fd, const char *name /*= "NA"*/)
    : _sock(fd)
    , _name(name)
{
    _addr.sun_family = AF_UNIX;
}

SocketDomain::SocketDomain(SocketDomain &&val)
{
    //std::cout << "SocketDomain(" << val._name << ")::SocketDomain && sock:" << val._sock << std::endl;

    _sock = val._sock;
    _addr = val._addr;
    _name = val._name;

    val._sock = -1;
}


SocketDomain::~SocketDomain()
{
    //std::cout << "SocketDomain(" << _name << ")::~SocketDomain sock:" << _sock  <<  std::endl;
    Release();
}

SocketDomain& SocketDomain::operator=(SocketDomain &&s)
{
    _sock = s._sock;
    _addr = s._addr;
    _name = s._name;

    s._sock = -1;
    return *this;
}

SocketResult SocketDomain::Init()
{
    //std::cout << "SocketDomain(" << _name << ")::Init sock:" << _sock << std::endl;
    Release();

    _sock = ::socket(AF_UNIX, SOCK_STREAM, 0);
    //std::cout << "SocketDomain(" << _name << ")::Init sock:" << _sock << std::endl;

    return (_sock == -1) ? SocketResult::SR_ERROR : SocketResult::SR_SUCCESS;
}

SocketResult SocketDomain::Init(const char *path)
{
    //std::cout << "SocketDomain(" << _name << ")::Init sock:" << _sock << std::endl;
    SocketResult res = Init();

    _addr.sun_family = AF_UNIX;
    strcpy(_addr.sun_path, path);

    return res;
}

SocketResult SocketDomain::InitServer(const char *path /*= nullptr*/)
{
    //std::cout << "SocketDomain(" << _name << ")InitServer sock:" << _sock << std::endl;
    SocketResult res = SocketResult::SR_ERROR;

    if (SocketResult::SR_SUCCESS == Init())
    {
        _addr.sun_family = AF_UNIX;
        if (path) std::strcpy(_addr.sun_path, path);

        unlink(_addr.sun_path);
        if (0 == ::bind(_sock, (struct sockaddr *) &_addr, sizeof (_addr)))
        {
            if (0 == ::listen(_sock, 150))
                res = SocketResult::SR_SUCCESS;
        }
    }

    return res;
}


int SocketDomain::Release()
{
//    std::cout << "SocketDomain(" << _name << ")::Release sock:" << _sock << std::endl;
    int rc = 0;
    if (_sock != -1)
    {
        //std::cout << "SocketDomain(" << _name << ") closing sock:" << _sock << std::endl;
        rc = ::close(_sock);
        _sock = -1;
    }
    return rc;
}

SocketResult SocketDomain::SetNonBlock()
{
    SocketResult res = SocketResult::SR_ERROR;

    int flags = fcntl(_sock, F_GETFL, 0);
    if (-1 == flags)
    {
        std::cerr << "ERROR : fcntl GETFL\n";
        return SocketResult::SR_ERROR;
    }

    flags |= O_NONBLOCK;
    if (-1 == fcntl(_sock, F_SETFL, flags))
    {
        std::cerr << "ERROR : fcntl SETFL\n";
        return SocketResult::SR_ERROR;
    }

    return SocketResult::SR_SUCCESS;
}


ssize_t SocketDomain::Read(void *pdata, size_t lenData)
{
    //std::cout << "SocketDomain(" << _name << ")::Read --->  sock:" << _sock << std::endl;

    ssize_t bytes = ::read(_sock, pdata, lenData);
    if (bytes < 0)
    {
        std::cerr << "SocketDomain(" << _name << ")::Read  ERROR - (" << errno << ") " << strerror(errno) << std::endl;
        Release();
    }

    if (bytes == 0)
    {
        std::cerr << "SocketDomain(" << _name << ")::Read  Peer disconnected\n";
        Release();
    }

    return bytes;
}

ssize_t  SocketDomain::Write(const void *pdata, size_t len)
{
    //std::cout << "SocketDomain(" << _name << ")::Write --->  sock:" << _sock << std::endl;

    ssize_t bytes = ::write(_sock, pdata, len);
    if (bytes == -1)
    {
        std::cerr << "SocketDomain(" << _name << ")::Write  ERROR - (" << errno << ") " << strerror(errno) << std::endl;
        Release();
    }
    if (bytes == 0)
    {
        std::cerr << "SocketDomain(" << _name << ")::Read  Peer disconnected\n";
        Release();
    }

    return bytes;
}
