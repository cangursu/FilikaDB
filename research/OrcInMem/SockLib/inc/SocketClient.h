/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SocketClient.h
 * Author: can.gursu
 *
 * Created on 01 August 2018, 16:22
 */

#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

#include "SocketResult.h"
#include "MemStream.h"
#include "SocketUtils.h"


#include <string>
#include <netdb.h>
#include <atomic>


template <typename TSocket>
class SocketClient : public TSocket
{
    public:
        SocketClient(const char *name)          : TSocket(name) {}
        SocketClient(int fd, const char *name)  : TSocket(fd, name) {}
        SocketClient(const SocketClient &val) = delete;
        SocketClient(SocketClient &&val)        : TSocket(std::move(val)) { }

        SocketClient& operator=(SocketClient &&s)   { TSocket::operator =(std::move(s));}
        SocketClient& operator=(const SocketClient &val) = delete;


        SocketResult Connect ();
        SocketResult Send    (const void *data, std::uint64_t len);
        SocketResult Send    (const MemStream<std::uint8_t> &)    ;

        SocketResult LoopRead();
        SocketResult LoopReadStop() { _exit = true; }



    public:
        // Events
        virtual void OnRecv        (MemStream<std::uint8_t> &&) = 0;
        virtual void OnErrorClient (SocketResult)               = 0;

    private:
        std::atomic<bool> _exit = false;
};



template <typename TSocket>
SocketResult SocketClient<TSocket>::Connect()
{
    SocketResult res = TSocket::Connect();
    if (res != SocketResult::SR_SUCCESS)
        OnErrorClient (res);
}

template <typename TSocket>
SocketResult SocketClient<TSocket>::Send(const void *data, std::uint64_t len)
{
    SocketResult res = SocketResult::SR_ERROR;

    if (TSocket::IsGood() && len > 0)
    {
        ssize_t i = 0;
        if (i = TSocket::Write(data, len) < 0)
        {
            OnErrorClient(res = SocketResult::SR_ERROR_SEND);
        }
        else
        {
            //std::cout << "i = " << i << "  errno = " <<  errno << " (" << strerror(errno) << ")\n";
            res = SocketResult::SR_SUCCESS;
        }
    }
    return res;
}

template <typename TSocket>
SocketResult SocketClient<TSocket>::Send(const MemStream<std::uint8_t> &stream)
{
    SocketResult result = SocketResult::SR_SUCCESS;

    const std::uint64_t lenBuff = 512L;
    std::uint64_t       len     = stream.Len();
    std::uint64_t       readed  = 0L;

    char buff[lenBuff] = "";
    for (int offset = 0; offset < len && (result == SocketResult::SR_SUCCESS); offset += readed)
    {
        readed = stream.read(buff, lenBuff, offset);
        result = Send(static_cast<void *>(buff), readed);
    }

    return result;
}


template <typename TSocket>
SocketResult SocketClient<TSocket>::LoopRead()
{
    std::cout << "Enter LoopRead - SocketClient<TSocket>\n";
    SocketResult result = SocketResult::SR_ERROR_AGAIN;
    if (TSocket::IsGood())
    {
        timeval tv { .tv_sec = 1, .tv_usec = 0 };
        if (0 > setsockopt(TSocket::fd(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv))
        {
            result = SocketResult::SR_ERROR_SOCKOPT;
        }
        else
        {
            const int buffLen = 512;
            uint8_t   buff[buffLen];
            while ((_exit == false) && (result == SocketResult::SR_ERROR_AGAIN))
            {
                MemStream<std::uint8_t> stream;

                ssize_t bytes = TSocket::Read(buff, buffLen);
                //std::cout << "bytes:" << bytes << " errno:" << ErrnoText(errno) << std::endl;

                if (bytes == 0)
                {
                    result = SocketResult::SR_ERROR_CONNECT;
                }
                else if (bytes < 0)
                {
                    //std::cout << "ERROR errno:" << errno << " - " << ErrnoText(errno) << std::endl;
                    switch (errno)
                    {
                        case EAGAIN : result = SocketResult::SR_ERROR_AGAIN; break;
                        default     : result = SocketResult::SR_ERROR_READ;  break;
                    }
                }
                else //(bytes > 0)
                {
                    stream.write(buff, bytes);
                    OnRecv(std::move(stream));
                }
            }
        }
    }

    if ( (_exit == true) && (result == SocketResult::SR_ERROR_AGAIN) )
        result = SocketResult::SR_SUCCESS; //Gracefully quit

    std::cout << "Leaveing LoopRead - SocketClient<TSocket> - " << SocketResultText(result) << std::endl;
    return result;
}




#endif /* __SOCKET_CLIENT_H__ */

