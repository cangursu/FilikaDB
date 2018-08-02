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


#include <string>
#include <netdb.h>


template <typename TSocket>
class SocketClient : public TSocket
{
    public:
        SocketResult Connect ();
        SocketResult Send    (const void *data, std::uint64_t len);
        SocketResult Send    (MemStream<std::uint8_t> &&)   ;

    public:
        // Events
        virtual void OnRecv        (const TSocket&, MemStream<std::uint8_t> &&) = 0;
        virtual void OnErrorClient (SocketResult)                               = 0;
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

    int fd = TSocket::fd();
    if (fd > 0 && len > 0)
    {
        int  i = 0;
        if (i = ::send(fd, data, len, 0) < 0)
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
SocketResult SocketClient<TSocket>::Send(MemStream<std::uint8_t> &&stream)
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




#endif /* __SOCKET_CLIENT_H__ */

