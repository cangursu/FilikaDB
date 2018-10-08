

#ifndef __SOCKET_TCP_H__
#define __SOCKET_TCP_H__


// *****************************************************
// Server / Client TCP Socket Implementation
//
//

#include "Socket.h"

#include <string>


class SocketTCP : public Socket
{
    public :
        SocketTCP(const char *name = "NA");
        SocketTCP(int fd, const char *name);
        SocketTCP(const std::string &addr, std::uint16_t port, const char *name);
        SocketTCP(const SocketTCP &s) = delete;
        SocketTCP(SocketTCP &&s)      = default;

        virtual ~SocketTCP();

        SocketTCP&         operator=(SocketTCP &&s)      = default;
        SocketTCP&         operator=(const SocketTCP &s) = delete;

        SocketResult       Init();
        SocketResult       InitServer();
        SocketResult       Connect();

        void               Port(std::uint16_t port)    { _port    = port;                                          }
        std::uint16_t      Port() const                { return _port;                                             }
        void               Address(const char *s)      { _address = s;                                             }
        void               Address(const char *s,
                                   std::uint16_t p)    { Address(s); Port(p);                                      }
        const std::string &Address() const             { return _address;                                          }
        std::string        PrmDesc()                   { return std::move(_address + ":" + std::to_string(_port)); }

        SocketResult       SetNonBlock();

        virtual ssize_t    Read  (void *pdata, size_t len);
        virtual ssize_t    Write (const void *pdata, size_t len);

private :
        std::uint16_t   _port    = -1;
        std::string     _address = "127.0.0.1";
};



#endif // __SOCKET_TCP_H__