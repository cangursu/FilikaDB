

#ifndef __SOCKET_H__
#define __SOCKET_H__


// *****************************************************
// Server / Client Socket Implementation
//
//

#include "MemStream.h"
#include "SocketResult.h"


#include <string>
#include <netdb.h>
#include <cstring>


class Socket
{
    public :
        Socket(int s);
        Socket()                = default;
        Socket(const Socket &s) = delete;
        Socket(Socket &&s);
        ~Socket();

        Socket& operator=(Socket &&s);

        SocketResult    Init();
        SocketResult    InitServer();
        void            Release();
        SocketResult    Connect();

        void            Port(uint16_t port)   { _port    = port; }
        void            Adress(const char *s) { _address = s;    }
        std::string     PrmDesc()             { return std::move(_address + ":" + std::to_string(_port));}

        SocketResult    SetNonBlock();

protected :
        int fd() const { return _sock; }
    private :
        int             _sock    = -1;
        uint16_t        _port    = -1;
        std::string     _address = "127.0.0.1";

};



#endif // __SOCKET_H__