
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Logger.cpp
 * Author: postgres
 * 
 * Created on April 19, 2018, 9:44 AM
 */


#include "Logger.h"

#include <cstdio>
#include <cstdarg>

#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <string>

namespace Filika
{

SockDomain::SockDomain(const char *path) 
{
    _addr.sun_family = AF_UNIX;
    strcpy(_addr.sun_path, path);
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
    _addr.sun_family = AF_UNIX;
    strcpy(_addr.sun_path, path);
    return init();
}


int SockDomain::initServer() 
{
    int rc = 0;
    release();

    if (init() == 0) 
    {
        unlink(_addr.sun_path);
        rc = bind(_sock, (struct sockaddr *) &_addr, sizeof (_addr));
    }

    return rc;
}

int SockDomain::release() 
{
    int rc = 0;
    if (_sock != -1) {
        rc = close(_sock);
        _sock = -1;
    }
    return rc;
}

int SockDomain::recvFrom(void *pdata, size_t lenData) 
{
    sockaddr_un peer_sock;
    socklen_t lenAddr = sizeof (peer_sock);

    int bytes = recvfrom(_sock, pdata, lenData, 0, (struct sockaddr *) &peer_sock, &lenAddr);
    if (bytes == -1)
    {
        release();
    }
    return bytes;
}

int SockDomain::sendTo(const void *pdata, size_t len) 
{
    int bytes = sendto(_sock, pdata, len, 0, (struct sockaddr *) &_addr, sizeof (_addr));
    if (bytes == -1) release();
    return bytes;

}






Logger::Logger()
{
    _sock.init();
}

Logger::Logger(const char *path)
{
    _sock.init(path);
}

Logger::~Logger()
{
    _sock.release();
}

FILIKA_RESULT Logger::send(const std::string &item)
{
    size_t len = item.length();
    int res = _sock.sendTo(item.c_str(), len);
    return res == (int)len ? FILIKA_RESULT::FR_ERROR : FILIKA_RESULT::FR_SUCCESS;
}

FILIKA_RESULT Logger::recv(std::string &item)
{
    static __thread  char buff[1024];
    FILIKA_RESULT res = FILIKA_RESULT::FR_ERROR;
    int len = _sock.recvFrom(buff, 1024);
    if (len > 0 && len < 1024)
    {
        res = FILIKA_RESULT::FR_SUCCESS;
        item.assign(buff, len);
    }
    return res;
}





void LogLine(Logger &lg, const char *fmt, ...)
{ 
    ::va_list list;
    ::va_start(list, fmt);

    char prbf[1024] = "";
    std::vsnprintf(prbf, 1024, fmt, list);
    lg.send(prbf); 
}

void LogLine(Logger &lg, const char *desc, const char *fmt, const char *funcName, ...)
{ 
    std::va_list list;
    va_start(list, funcName);

    char prbf[1024] = "";
    std::snprintf(prbf, 1024, "%-20s : %-35s : %s\n", funcName, desc, fmt);
    
    
    char bf[1024] = ""; 
    vsnprintf(bf, 1024, prbf,  list); 
    lg.send(bf); 
}

void LogLine(Logger &lg, const char *desc, const char *fmt, int lineNo, const char *fname, const char *funcName, ...)
{ 
    std::va_list list;
    va_start(list, funcName);

    char prbf[1024] = "";
    std::snprintf(prbf, 1024, "%03d: %-28s: %-20s : %-35s : %s\n", lineNo, fname, funcName, desc, fmt);

    char bf[1024] = ""; 
    vsnprintf(bf, 1024, prbf,  list); 
    lg.send(bf); 
}


}
