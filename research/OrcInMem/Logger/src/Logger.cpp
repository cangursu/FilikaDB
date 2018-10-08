
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Logger.cpp
 * Author: can.gursu@medyasoft.com.tr
 *
 * Created on April 19, 2018, 9:44 AM
 */


#include "Logger.h"
#include "StreamPacket.h"
#include "GeneralUtils.h"

#include <cstdio>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <string>

namespace Filika
{



LSockLog::LSockLog(const char *path/* = SOCK_PATH_DEFAULT*/, bool doConnect/* = true*/)
        : SocketDomain("SockLog")
{
    SocketPath(path);
    Name("SockLog");

    if (SocketResult::SR_SUCCESS != Init())
    {
        std::cerr << "ERROR : Unable to connect init log\n";
    }
    else
    {
        if (doConnect && path && *path)
        {
            if (SocketResult::SR_SUCCESS != Connect())
            {
                std::cerr << "ERROR : Unable to connect Connect Log Server (" << path << ")\n";
                Release();
            }
        }
    }
}

LSockLog::~LSockLog()
{
    //SendRaw();
}

SocketResult LSockLog::SendRaw()
{
    std::string s   = _os.str();
    size_t      len = s.length();

    SendRaw(s.c_str(), s.length() + 1);
}

SocketResult LSockLog::SendPacket()
{
    if (SocketPath().empty()) return SocketResult::SR_EMPTY;

    std::string s   = _os.str();
    StreamPacket pack(s.c_str(), s.length());

    const StreamPacket::byte_t *p = nullptr;
    std::uint32_t               l = pack.Buffer(&p);

    return SendRaw(p,l);
}

SocketResult LSockLog::SendRaw(const void *p, int l)
{
    if (SocketPath().empty() || (l < 1)) return SocketResult::SR_EMPTY;
    if (nullptr == p)                    return SocketResult::SR_ERROR_PRM;

    ssize_t res = 0;
    if (!IsGood())
    {
        if (SocketResult::SR_SUCCESS != Connect())
            std::cerr << "ERROR : Unable to connect (" << errno << ") " << strerror(errno) << std::endl;
    }
    if (IsGood())
    {
        //std::cerr << "Write  ->" << std::endl;
        res = Write(p, l);
        //std::cerr << "Write  -< res:" << res << std::endl;
    }

    Reset();

    return ( (res == -1) || (res != (ssize_t)(l))) ? SocketResult::SR_ERROR : SocketResult::SR_SUCCESS;
}










}
