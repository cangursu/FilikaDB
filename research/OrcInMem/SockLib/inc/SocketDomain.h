/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SocketDomain.h
 * Author: can.gursu
 *
 * Created on 13 July 2018, 09:16
 */

#ifndef __SOCKET_DOMAIN_H__
#define __SOCKET_DOMAIN_H__

#include <sys/un.h>
#include <sys/types.h>

class SockDomain
{
    public:

    SockDomain()                    = default;
    SockDomain(const SockDomain &)  = default; //= delete;
    SockDomain(SockDomain &&)       = delete;
    SockDomain(const char *path);
    virtual ~SockDomain() ;

    int init();
    int init(const char *path);
    int initServer();
    int release() ;

    ssize_t recvFrom(void *pdata, size_t len) ;
    ssize_t sendTo  (const void *pdata, size_t len);

private:
    int             _sock = -1;
    sockaddr_un     _addr/*{AF_UNIX, SOCK_PATH_DEFAULT}*/;
};




#endif // __SOCKET_DOMAIN_H__

