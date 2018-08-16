

#ifndef __SOCKET_TCP_H__
#define __SOCKET_TCP_H__


// *****************************************************
// Server / Client TCP Socket Implementation
//
//

//#include "MemStream.h"
#include "SocketResult.h"


#include <string>
#include <netdb.h>
#include <cstring>


class SocketTCP
{
    public :
        SocketTCP(int s, const char *name);
        SocketTCP(const char *name = "NA");
        SocketTCP(const SocketTCP &s) = delete;
        SocketTCP(SocketTCP &&s);
        ~SocketTCP();

        SocketTCP& operator=(SocketTCP &&s);

        SocketResult    Init();
        SocketResult    InitServer();
        void            Release();
        SocketResult    Connect();

        void            Port(uint16_t port)   { _port    = port;                                          }
        void            Adress(const char *s) { _address = s;                                             }
        std::string     PrmDesc()             { return std::move(_address + ":" + std::to_string(_port)); }
        bool            IsGood ()             { return _sock != -1;                                       }

        SocketResult    SetNonBlock();

protected :
        int fd() const { return _sock; }
private :
        int             _sock    = -1;
        uint16_t        _port    = -1;
        std::string     _address = "127.0.0.1";

        std::string     _name    = "NA";
};



#endif // __SOCKET_TCP_H__