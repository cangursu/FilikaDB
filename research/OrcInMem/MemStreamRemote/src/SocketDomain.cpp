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


#include <sys/socket.h>
#include <unistd.h>



SockDomain::SockDomain(const char *path)
{
    ____addr.sun_family = AF_UNIX;
    strcpy(____addr.sun_path, path);
}

SockDomain::~SockDomain()
{
    release();
}

int SockDomain::init()
{
    release();
    _sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    return _sock == -1 ? -1 : 0;
}

int SockDomain::init(const char *path)
{
    ____addr.sun_family = AF_UNIX;
    strcpy(____addr.sun_path, path);
    return init();
}


int SockDomain::initServer()
{
    int rc = 0;
    release();

    if (init() == 0)
    {
        unlink(____addr.sun_path);
        rc = bind(_sock, (struct sockaddr *) &____addr, sizeof (____addr));
    }

    return rc;
}

int SockDomain::release()
{
    int rc = 0;
    if (_sock != -1)
    {
        rc = close(_sock);
        _sock = -1;
    }
    return rc;
}

ssize_t SockDomain::recvFrom(void *pdata, size_t lenData)
{
    sockaddr_un peer_sock;
    socklen_t lenAddr = sizeof (peer_sock);

    ssize_t bytes = recvfrom(_sock, pdata, lenData, 0, (struct sockaddr *) &peer_sock, &lenAddr);
    if (bytes == -1) release();

    return bytes;
}

ssize_t  SockDomain::sendTo(const void *pdata, size_t len)
{
    ssize_t bytes = sendto(_sock, pdata, len, 0, (struct sockaddr *) &____addr, sizeof (____addr));
    if (bytes == -1) release();
    return bytes;
}
