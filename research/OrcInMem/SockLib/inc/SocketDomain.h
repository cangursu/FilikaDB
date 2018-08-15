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

#include "SocketResult.h"

#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>



#define SOCK_PATH_DEFAULT "unix_sock.server"



class SocketDomain
{
public:

    SocketDomain(int fd, const char *name /*= "NA"*/);
    SocketDomain(const char *name = "NA");
    SocketDomain(const SocketDomain &val) = delete;
    SocketDomain(SocketDomain &&val);

    virtual ~SocketDomain() ;

    SocketDomain& operator=(SocketDomain &&s);
    SocketDomain& operator=(const SocketDomain &val) = delete;

    SocketResult Init();
    SocketResult Init(const char *path);
    SocketResult InitServer(const char *path = nullptr);


    int          Release() ;

    // Parameters
    const std::string &Name()                      { return _name; }
    void         Name(const char *name)            { _name = name; }
    void         SocketPath(const char *spath)     { strcpy(_addr.sun_path, spath);  _addr.sun_family = AF_UNIX; }
    std::string  PrmDesc()                         { return std::move(std::string(_addr.sun_path)); }
    SocketResult SetNonBlock();

    bool         IsGood   ()                       { return _sock != -1;                                                      }
    SocketResult Connect  ()                       { return (::connect(_sock, (struct sockaddr*) &_addr, sizeof (_addr)) == 0) ? SocketResult::SR_SUCCESS : SocketResult::SR_ERROR_CONNECT; }
    SocketDomain Accept   ()                       { return std::move(SocketDomain(::accept(_sock, NULL, NULL), "client"));   }
    int          AcceptFd ()                       { return ::accept(_sock, NULL, NULL);                                      }
    ssize_t      Read     (void *pdata, size_t len);
    ssize_t      Write    (const void *pdata, size_t len);


protected :
    int fd() const { return _sock; }
private:
    int             _sock = -1;
    sockaddr_un     _addr{AF_UNIX, SOCK_PATH_DEFAULT};

    std::string     _name = "NA";
};




#endif // __SOCKET_DOMAIN_H__

