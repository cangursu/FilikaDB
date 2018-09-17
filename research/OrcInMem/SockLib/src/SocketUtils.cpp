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
    const char *res = "NA";
    switch(val)
    {
        case SocketResult::SR_TIMEOUT         : res = "SR_TIMEOUT"          ; break;
        case SocketResult::SR_EMPTY           : res = "SR_EMPTY"            ; break;
        case SocketResult::SR_SUCCESS         : res = "SR_SUCCESS"          ; break;
        case SocketResult::SR_ERROR           : res = "SR_ERROR"            ; break;
        case SocketResult::SR_ERROR_PRM       : res = "SR_ERROR_PRM"        ; break;
        case SocketResult::SR_ERROR_EOF       : res = "SR_ERROR_EOF"        ; break;
        case SocketResult::SR_ERROR_BIND      : res = "SR_ERROR_BIND"       ; break;
        case SocketResult::SR_ERROR_ACCEPT    : res = "SR_ERROR_ACCEPT"     ; break;
        case SocketResult::SR_ERROR_CONNECT   : res = "SR_ERROR_CONNECT"    ; break;
        case SocketResult::SR_ERROR_SEND      : res = "SR_ERROR_SEND"       ; break;
        case SocketResult::SR_ERROR_READ      : res = "SR_ERROR_READ"       ; break;
        case SocketResult::SR_ERROR_EPOLL     : res = "SR_ERROR_EPOLL"      ; break;
        case SocketResult::SR_ERROR_LEN       : res = "SR_ERROR_LEN"        ; break;
        case SocketResult::SR_ERROR_AGAIN     : res = "SR_ERROR_AGAIN"      ; break;
        case SocketResult::SR_ERROR_REUSEADDR : res = "SR_ERROR_REUSEADDR"  ; break;
        case SocketResult::SR_ERROR_SOCKOPT   : res = "SR_ERROR_SOCKOPT"    ; break;
        case SocketResult::SR_ERROR_CRC       : res = "SR_ERROR_CRC"        ; break;
        case SocketResult::SR_ERROR_NOTCLIENT : res = "SR_ERROR_NOTCLIENT"  ; break;
        case SocketResult::SR_ERROR_PARSE     : res = "SR_ERROR_PARSE"      ; break;
    }
    return res;
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


const char *ErrnoText(int val)
{
    switch (val)
    {
        case EPERM           : return "EPERM"              ;
        case ENOENT          : return"ENOENT"              ;
        case ESRCH           : return"ESRCH"               ;
        case EINTR           : return"EINTR"               ;
        case EIO             : return"EIO"                 ;
        case ENXIO           : return"ENXIO"               ;
        case E2BIG           : return"E2BIG"               ;
        case ENOEXEC         : return"ENOEXEC"             ;
        case EBADF           : return"EBADF"               ;
        case ECHILD          : return"ECHILD"              ;
        case EAGAIN          : return"EAGAIN/EWOULDBLOCK"  ;
        case ENOMEM          : return"ENOMEM"              ;
        case EACCES          : return"EACCES"              ;
        case EFAULT          : return"EFAULT"              ;
        case ENOTBLK         : return"ENOTBLK"             ;
        case EBUSY           : return"EBUSY"               ;
        case EEXIST          : return"EEXIST"              ;
        case EXDEV           : return"EXDEV"               ;
        case ENODEV          : return"ENODEV"              ;
        case ENOTDIR         : return"ENOTDIR"             ;
        case EISDIR          : return"EISDIR"              ;
        case EINVAL          : return"EINVAL"              ;
        case ENFILE          : return"ENFILE"              ;
        case EMFILE          : return"EMFILE"              ;
        case ENOTTY          : return"ENOTTY"              ;
        case ETXTBSY         : return"ETXTBSY"             ;
        case EFBIG           : return"EFBIG"               ;
        case ENOSPC          : return"ENOSPC"              ;
        case ESPIPE          : return"ESPIPE"              ;
        case EROFS           : return"EROFS"               ;
        case EMLINK          : return"EMLINK"              ;
        case EPIPE           : return"EPIPE"               ;
        case EDOM            : return"EDOM"                ;
        case ERANGE          : return"ERANGE"              ;

        case EDEADLK          : return "EDEADLK/EDEADLOCK" ;
        case ENAMETOOLONG     : return "ENAMETOOLONG"      ;
        case ENOLCK           : return "ENOLCK"            ;
        case ENOSYS           : return "ENOSYS"            ;
        case ENOTEMPTY        : return "ENOTEMPTY"         ;
        case ELOOP            : return "ELOOP"             ;
//      case EWOULDBLOCK      : return "EWOULDBLOCK"       ;
        case ENOMSG           : return "ENOMSG"            ;
        case EIDRM            : return "EIDRM"             ;
        case ECHRNG           : return "ECHRNG"            ;
        case EL2NSYNC         : return "EL2NSYNC"          ;
        case EL3HLT           : return "EL3HLT"            ;
        case EL3RST           : return "EL3RST"            ;
        case ELNRNG           : return "ELNRNG"            ;
        case EUNATCH          : return "EUNATCH"           ;
        case ENOCSI           : return "ENOCSI"            ;
        case EL2HLT           : return "EL2HLT"            ;
        case EBADE            : return "EBADE"             ;
        case EBADR            : return "EBADR"             ;
        case EXFULL           : return "EXFULL"            ;
        case ENOANO           : return "ENOANO"            ;
        case EBADRQC          : return "EBADRQC"           ;
        case EBADSLT          : return "EBADSLT"           ;
//      case EDEADLOCK        : return "EDEADLOCK"         ;
        case EBFONT           : return "EBFONT"            ;
        case ENOSTR           : return "ENOSTR"            ;
        case ENODATA          : return "ENODATA"           ;
        case ETIME            : return "ETIME"             ;
        case ENOSR            : return "ENOSR"             ;
        case ENONET           : return "ENONET"            ;
        case ENOPKG           : return "ENOPKG"            ;
        case EREMOTE          : return "EREMOTE"           ;
        case ENOLINK          : return "ENOLINK"           ;
        case EADV             : return "EADV"              ;
        case ESRMNT           : return "ESRMNT"            ;
        case ECOMM            : return "ECOMM"             ;
        case EPROTO           : return "EPROTO"            ;
        case EMULTIHOP        : return "EMULTIHOP"         ;
        case EDOTDOT          : return "EDOTDOT"           ;
        case EBADMSG          : return "EBADMSG"           ;
        case EOVERFLOW        : return "EOVERFLOW"         ;
        case ENOTUNIQ         : return "ENOTUNIQ"          ;
        case EBADFD           : return "EBADFD"            ;
        case EREMCHG          : return "EREMCHG"           ;
        case ELIBACC          : return "ELIBACC"           ;
        case ELIBBAD          : return "ELIBBAD"           ;
        case ELIBSCN          : return "ELIBSCN"           ;
        case ELIBMAX          : return "ELIBMAX"           ;
        case ELIBEXEC         : return "ELIBEXEC"          ;
        case EILSEQ           : return "EILSEQ"            ;
        case ERESTART         : return "ERESTART"          ;
        case ESTRPIPE         : return "ESTRPIPE"          ;
        case EUSERS           : return "EUSERS"            ;
        case ENOTSOCK         : return "ENOTSOCK"          ;
        case EDESTADDRREQ     : return "EDESTADDRREQ"      ;
        case EMSGSIZE         : return "EMSGSIZE"          ;
        case EPROTOTYPE       : return "EPROTOTYPE"        ;
        case ENOPROTOOPT      : return "ENOPROTOOPT"       ;
        case EPROTONOSUPPORT  : return "EPROTONOSUPPORT"   ;
        case ESOCKTNOSUPPORT  : return "ESOCKTNOSUPPORT"   ;
        case EOPNOTSUPP       : return "EOPNOTSUPP"        ;
        case EPFNOSUPPORT     : return "EPFNOSUPPORT"      ;
        case EAFNOSUPPORT     : return "EAFNOSUPPORT"      ;
        case EADDRINUSE       : return "EADDRINUSE"        ;
        case EADDRNOTAVAIL    : return "EADDRNOTAVAIL"     ;
        case ENETDOWN         : return "ENETDOWN"          ;
        case ENETUNREACH      : return "ENETUNREACH"       ;
        case ENETRESET        : return "ENETRESET"         ;
        case ECONNABORTED     : return "ECONNABORTED"      ;
        case ECONNRESET       : return "ECONNRESET"        ;
        case ENOBUFS          : return "ENOBUFS"           ;
        case EISCONN          : return "EISCONN"           ;
        case ENOTCONN         : return "ENOTCONN"          ;
        case ESHUTDOWN        : return "ESHUTDOWN"         ;
        case ETOOMANYREFS     : return "ETOOMANYREFS"      ;
        case ETIMEDOUT        : return "ETIMEDOUT"         ;
        case ECONNREFUSED     : return "ECONNREFUSED"      ;
        case EHOSTDOWN        : return "EHOSTDOWN"         ;
        case EHOSTUNREACH     : return "EHOSTUNREACH"      ;
        case EALREADY         : return "EALREADY"          ;
        case EINPROGRESS      : return "EINPROGRESS"       ;
        case ESTALE           : return "ESTALE"            ;
        case EUCLEAN          : return "EUCLEAN"           ;
        case ENOTNAM          : return "ENOTNAM"           ;
        case ENAVAIL          : return "ENAVAIL"           ;
        case EISNAM           : return "EISNAM"            ;
        case EREMOTEIO        : return "EREMOTEIO"         ;
        case EDQUOT           : return "EDQUOT"            ;
        case ENOMEDIUM        : return "ENOMEDIUM"         ;
        case EMEDIUMTYPE      : return "EMEDIUMTYPE"       ;
        case ECANCELED        : return "ECANCELED"         ;
        case ENOKEY           : return "ENOKEY"            ;
        case EKEYEXPIRED      : return "EKEYEXPIRED"       ;
        case EKEYREVOKED      : return "EKEYREVOKED"       ;
        case EKEYREJECTED     : return "EKEYREJECTED"      ;
        case EOWNERDEAD       : return "EOWNERDEAD"        ;
        case ENOTRECOVERABLE  : return "ENOTRECOVERABLE"   ;
        case ERFKILL          : return "ERFKILL"           ;
        case EHWPOISON        : return "EHWPOISON"         ;

    }
    return "NA";
}