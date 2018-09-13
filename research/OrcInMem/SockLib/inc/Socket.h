

#ifndef __SOCKET_H__
#define __SOCKET_H__


// *****************************************************
// Socket Based Implementation
//
//

#include "SocketResult.h"

#include <string>


class Socket
{
    public :
        Socket(int s, const char *name);
        Socket(const char *name = "NA");
        Socket(const Socket &s) = delete;
        Socket(Socket &&s);
        ~Socket();

        Socket&         operator= (Socket &&s);
        Socket&         operator= (const Socket &s) = delete;

        SocketResult    Init (int domain, int type);
        SocketResult    Release ();
        bool            IsGood ()                      { return _sock != -1; }

        std::string     Name() const                   { return _name;       }
        void            Name (const std::string &val)  { _name = val;        }


        virtual ssize_t Read  (void *pdata, size_t len)        = 0;
        virtual ssize_t Write (const void *pdata, size_t len)  = 0;


//protected :  ??? :(
        int fd() const                                 { return _sock;       }

private :
        int          _sock    = -1;
        std::string  _name    = "NA";
};



#endif // __SOCKET_H__