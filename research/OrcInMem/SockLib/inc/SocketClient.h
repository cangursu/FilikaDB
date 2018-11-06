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
#include "GeneralUtils.h"
#include "Loop.h"


#include <string>
#include <netdb.h>
#include <atomic>
#include <ios>


template <typename TSocket>
class SocketClient : public TSocket
{
    public:
        SocketClient(const char *name)          : TSocket(name)     , _loop(name)   {}
        SocketClient(int fd, const char *name)  : TSocket(fd, name) , _loop(name)   {}
        SocketClient(const SocketClient &val) = delete;
        SocketClient(SocketClient &&val)        : TSocket(std::move(val)) , _loop(val.Name())
        {
        }

        SocketClient& operator=(SocketClient &&s)   { TSocket::operator =(std::move(s)); }
        SocketClient& operator=(const SocketClient &val) = delete;


        SocketResult ConnectServer ();
        SocketResult Send          (const void *data, std::uint64_t len);
        SocketResult Send          (const MemStream<std::uint8_t> &)    ;


        SocketResult LoopStart();
        void         LoopStop() {_loop.Stop();}
        bool         LoopStat() {_loop.Stat();}

    public:
        // Events
        virtual void OnRecv        (MemStream<std::uint8_t> &&) = 0;
        virtual void OnErrorClient (SocketResult)               = 0;

    private:
        std::atomic<bool> _exit = false;
        Loop              _loop;
};


template <typename TSocket>
SocketResult SocketClient<TSocket>::ConnectServer()
{
    SocketResult res = SocketResult::SR_EMPTY;

    errno = 0;
    int ntry = 0;
    for (ntry = 0; (res != SocketResult::SR_SUCCESS) && (ntry < 5); ++ntry)
    {
        res = this->Connect();
        nsleep(10);
    }

    if (res != SocketResult::SR_SUCCESS)
    {
        OnErrorClient (res);
    }

    return res;
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
            res = SocketResult::SR_SUCCESS;
#ifdef FOLLOW_RAWDATA_FLOW
            std::cout << "i = " << i << "  errno = " <<  errno << " (" << strerror(errno) << ")\n";
            std::cout << "Send Data  (length:" << len << ") ->\n" << DumpMemory(data, len) << std::endl;
#endif //FOLLOW_RAWDATA_FLOW
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
        readed = stream.Read(buff, lenBuff, offset);
        result = Send(static_cast<void *>(buff), readed);
    }

    return result;
}

template <typename TSocket>
SocketResult SocketClient<TSocket>::LoopStart()
{
    SocketResult result = SocketResult::SR_ERROR;

    std::function<bool()> fncLoop = [this, &result]()->bool
    {
        const int buffLen = 512;
        uint8_t   buff[buffLen];

        MemStream<std::uint8_t> stream;
        ssize_t bytes = TSocket::Read(buff, buffLen);

        if (bytes == 0)
        {
            result = SocketResult::SR_ERROR_CONNECT;
        }
        else if (bytes < 0)
        {
            switch (errno)
            {
                case EAGAIN     : result = SocketResult::SR_ERROR_AGAIN; break;
                default         : result = SocketResult::SR_ERROR_READ;  break;
            }
        }
        else //(bytes > 0)
        {
            stream.Write(buff, bytes);
            OnRecv(std::move(stream));

            result = SocketResult::SR_ERROR_AGAIN;
        }

        return result == SocketResult::SR_ERROR_AGAIN;
    };



    std::function<bool()> fncPre  = [this, &result]()->bool
    {
        bool res = false;

        if (this->IsGood())
        {
            timeval tv { .tv_sec = 1, .tv_usec = 0 };
            res = (0 == setsockopt(TSocket::fd(), SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(timeval)));
            if (false == res)
            {
                std::cerr << "setsockopt : " << ErrnoText(errno) << " (" << errno << ")\n";
                result = SocketResult::SR_ERROR_SOCKOPT;
            }
        }

        return res;
    };



    _loop.Start(fncLoop, fncPre);
    if ( (_loop.Stat() == true) || (result == SocketResult::SR_ERROR_AGAIN) )
        result = SocketResult::SR_SUCCESS; //Gracefully quit

    return result;
}



#endif /* __SOCKET_CLIENT_H__ */

