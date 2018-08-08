/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "SocketUtils.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <sys/epoll.h>
#include <netdb.h>



bool NameInfo(const sockaddr &addr, std::string &host, std::string &serv) /*const*/
{
    bool res = false;

    socklen_t in_len = sizeof(addr);
    char bufHost[NI_MAXHOST];
    char bufServ[NI_MAXSERV];

    if (getnameinfo(&addr, in_len,
                    bufHost, NI_MAXHOST,
                    bufServ, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICHOST) == 0)
    {
        res = true;
        host = bufHost;
        serv = bufServ;
    }

    return res;
}






std::string EPollEvents(std::uint32_t eid)
{
    std::stringstream ss;
    for (std::uint32_t id = 1; id > 0; id <<= 1)
    {
        const char *s = EPollEventsText(eid & id);
        if (0 != std::strcmp(s, "NA"))
        {
            if (ss.tellp() > 0) ss << ", ";
            ss << s;
        }
    }

    return std::move(ss.str());
}

const char *EPollEventsText(std::uint32_t eid)
{
    if ( eid & EPOLLIN     ) return "EPOLLIN"       ;
    if ( eid & EPOLLPRI    ) return "EPOLLPRI"      ;
    if ( eid & EPOLLOUT    ) return "EPOLLOUT"      ;
    if ( eid & EPOLLRDNORM ) return "EPOLLRDNORM"   ;
    if ( eid & EPOLLRDBAND ) return "EPOLLRDBAND"   ;
    if ( eid & EPOLLWRNORM ) return "EPOLLWRNORM"   ;
    if ( eid & EPOLLWRBAND ) return "EPOLLWRBAND"   ;
    if ( eid & EPOLLMSG    ) return "EPOLLMSG"      ;
    if ( eid & EPOLLERR    ) return "EPOLLERR"      ;
    if ( eid & EPOLLHUP    ) return "EPOLLHUP"      ;
    if ( eid & EPOLLRDHUP  ) return "EPOLLRDHUP"    ;
    if ( eid & EPOLLWAKEUP ) return "EPOLLWAKEUP"   ;
    if ( eid & EPOLLONESHOT) return "EPOLLONESHOT"  ;
    if ( eid & EPOLLET     ) return "EPOLLET"       ;

    return "NA";
}


const char *SocketResultText(const SocketResult &val)
{
    switch(val)
    {
        case SocketResult::SR_SUCCESS         : return "SUCCESS"        ;
        case SocketResult::SR_ERROR           : return "ERROR"          ;
        case SocketResult::SR_ERROR_BIND      : return "ERROR_BIND"     ;
        case SocketResult::SR_ERROR_CONNECT   : return "ERROR_CONNECT"  ;
        case SocketResult::SR_ERROR_SEND      : return "ERROR_SEND"     ;
        case SocketResult::SR_ERROR_READ      : return "ERROR_READ"     ;
        case SocketResult::SR_ERROR_EPOLL     : return "ERROR_EPOLL"    ;
        case SocketResult::SR_ERROR_LEN       : return "ERROR_LEN"      ;
        case SocketResult::SR_ERROR_AGAIN     : return "ERROR_AGAIN"    ;
        case SocketResult::SR_ERROR_REUSEADDR : return "ERROR_REUSEADDR";
    }
    return "NA";
};


const char *ParseResultText(const ParseResult &val)
{
    switch (val)
    {
        case ParseResult::ERROR    : return "ERROR"     ;
        case ParseResult::CONTINUE : return "CONTINUE"  ;
        case ParseResult::NEXT     : return "NEXT"      ;
        case ParseResult::FINISH   : return "FINISH"    ;
    }
    return "NA";
};

