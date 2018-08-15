
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

#include <cstdio>

#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <string>

namespace Filika
{



LSockLog::LSockLog(const char *path/* = SOCK_PATH_DEFAULT*/, bool doConnect/* = true*/)
        : SocketDomain("SockLog")
        , _doConnect(doConnect)
{
    SocketPath(path);
    Name("SockLog");

    if (SocketResult::SR_SUCCESS != Init())
    {
        std::cerr << "ERROR : Unable to connect init log\n";
    }
    else
    {
        if (_doConnect)
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

FILIKA_RESULT LSockLog::SendRaw()
{
    std::string s   = _os.str();
    size_t      len = s.length();

    SendRaw(s.c_str(), s.length() + 1);
    /*
    std::string s   = _os.str();
    size_t      len = s.length();
    ssize_t     res = 0;

    if (len > 0)
    {
        if (true  == IsGood())
        {
            if (_doConnect  || (SocketResult::SR_SUCCESS == Connect()))
                 res   = Write(s.c_str(), len + 1 );
            else
                std::cerr << "Unable to connect (" << errno << ") " << strerror(errno) << std::endl;
        }
    }
    Reset();

    return ( (res == -1) || (res != (ssize_t)(len+1))) ? FILIKA_RESULT::FR_ERROR : FILIKA_RESULT::FR_SUCCESS;
    */
}

FILIKA_RESULT LSockLog::SendPacket()
{
    std::string s   = _os.str();
    StreamPacket pack(s.c_str(), s.length());

    const StreamPacket::byte_t *p = nullptr;
    std::uint32_t               l = pack.Buffer(&p);

    return SendRaw(p,l);
}

FILIKA_RESULT LSockLog::SendRaw(const void *p, int l)
{
    ssize_t     res = 0;
    if (l > 0)
    {
        if (true  == IsGood())
        {
            if (_doConnect  || (SocketResult::SR_SUCCESS == Connect()))
                 res   = Write(p, l);
            else
                std::cerr << "Unable to connect (" << errno << ") " << strerror(errno) << std::endl;
        }
    }
    Reset();

    return ( (res == -1) || (res != (ssize_t)(l))) ? FILIKA_RESULT::FR_ERROR : FILIKA_RESULT::FR_SUCCESS;
}










}
