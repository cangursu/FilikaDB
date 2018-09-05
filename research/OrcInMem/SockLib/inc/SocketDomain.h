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

#include "Socket.h"

#include <string>


#define SOCK_PATH_DEFAULT "unix_sock.server"



class SocketDomain : public Socket
{
public:

    SocketDomain(const char *name = "NA");
    SocketDomain(int fd, const char *name /*= "NA"*/);
    SocketDomain(const SocketDomain &) = delete;
    SocketDomain(SocketDomain &&val);

    virtual ~SocketDomain();

    SocketDomain& operator=(SocketDomain &&s);
    SocketDomain& operator=(const SocketDomain &val) = delete;

    SocketResult  Init();
    SocketResult  Init(const char *path);
    SocketResult  InitServer(const char *path = nullptr);

    // Parameters
    void                SocketPath  (const char *spath)   { if (spath) _path = spath; }
    const std::string & SocketPath  ()                    { return _path;             }
    std::string         PrmDesc     ()                    { return _path;             }

    SocketResult        SetNonBlock ();
    SocketResult        Connect     ();
    SocketDomain        Accept      ();
    int                 AcceptFd    ();
    virtual ssize_t     Read        (void *pdata, size_t len);
    virtual ssize_t     Write       (const void *pdata, size_t len);

private:
    std::string  _path;// = SOCK_PATH_DEFAULT;
};




#endif // __SOCKET_DOMAIN_H__

