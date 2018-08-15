/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef __SOCKET_UTILITIES_H__
#define __SOCKET_UTILITIES_H__


#include "SocketResult.h"
#include "ParseResult.h"


#include <cstdint>
#include <string>
#include <sys/socket.h>


bool            NameInfo            (const sockaddr &addr, std::string &host, std::string &serv);

const char *    ErrnoText           (int val);

const char *    EPollEventsText     (std::uint32_t eid);
std::string     EPollEvents         (std::uint32_t eid);

const char *    SocketResultText    (const SocketResult &val);
const char *    ParseResultText     (const ParseResult  &val);




#endif //__SOCKET_UTILITIES_H__