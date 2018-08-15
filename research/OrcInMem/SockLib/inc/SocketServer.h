/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SocketServer.h
 * Author: can.gursu
 *
 * Created on 01 August 2018, 15:56
 */

#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include "SocketResult.h"
//#include "SocketUtils.h"
#include "MemStream.h"


#include <unordered_map>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <atomic>





#define MAXEVENTS 1024






template <typename TSocketSrv, typename TSocketClt>
class SocketServer  : public TSocketSrv
{
    public:
        SocketServer(const char *name) : TSocketSrv(name) {}

        SocketResult    Init();
        SocketResult    Release();

        SocketResult    LoopListen();
        void            LoopListenStop() { _exit = true; }

        void            Accept();
        virtual void    Recv(int fd);
        void            Disconnect(int fd);

        std::uint64_t   ClientCount() { return _clientList.size(); }


    public:
        // Events
        virtual void OnAccept      (const TSocketClt &, const sockaddr &)           = 0;
        virtual void OnRecv        (TSocketClt &,       MemStream<std::uint8_t> &&) = 0;
        virtual void OnDisconnect  (const TSocketClt &)                             = 0;
        virtual void OnErrorClient (SocketResult)                                   = 0;
        virtual void OnErrorServer (SocketResult)                                   = 0;

    protected :
        int epoll() const  { return _epoll; }

    private :
        class FdMap
        {
            public  :
                bool erase(int fd, TSocketClt &s)
                {
                    auto it = _map.find(fd);
                    if (it != _map.end())
                    {
                        s = std::move(it->second);
                        _map.erase(it);
                        _size--;
                        return true;
                    }
                    return false;
                }
                bool set(int fd, TSocketClt &&s)
                {
                    _map[fd] = std::move(s);
                    _size++;
                    return true;
                }

                TSocketClt* get(int fd)
                {
                    auto it = _map.find(fd);
                    if (it != _map.cend())
                        return &it->second;
                    return nullptr;
                }
                const TSocketClt* get(int fd) const
                {
                    auto it = _map.find(fd);
                    if (it != _map.cend())
                        return &it->second;
                    return nullptr;
                }

                std::uint64_t size () { return _size; }
            private :
                std::unordered_map<int, TSocketClt> _map;
                std::uint64_t                       _size = 0;
                //TSocketClt                          _dummy;
        } _clientList;


        int                 _epoll = -1;
        std::atomic<bool>   _exit  = false;
};






template <typename TSocketSrv, typename TSocketClt>
SocketResult SocketServer<TSocketSrv, TSocketClt>::Init ()
{
    SocketResult res = TSocketSrv::InitServer();
    if(SocketResult::SR_SUCCESS != res)
        OnErrorServer(res);

    return res;
}

template <typename TSocketSrv, typename TSocketClt>
SocketResult SocketServer<TSocketSrv, TSocketClt>::Release()
{
    if (_epoll != -1)
    {
        ::close(_epoll);
        _epoll = -1;
    }
    TSocketSrv::Release();
}

template <typename TSocketSrv, typename TSocketClt>
SocketResult SocketServer<TSocketSrv, TSocketClt>::LoopListen()
{
    TSocketSrv::SetNonBlock();

    if (::listen(TSocketSrv::fd(), 1024) == -1)
    {
        std::cerr << "Listen\n";
        return SocketResult::SR_ERROR;
    }

    std::cout << "SocketServer Listenin : " << TSocketSrv::PrmDesc() << std::endl;

    if (-1 == (_epoll = epoll_create1(0)))
    {
        std::cerr << "epoll_create1\n";
        return SocketResult::SR_ERROR;
    }

    //epoll_event event {.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET | EPOLLONESHOT | EPOLLWAKEUP | EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG /*| EPOLLEXCLUSIVE*/};
    epoll_event event {.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET | EPOLLMSG};
    event.data.fd = TSocketSrv::fd();

    if (epoll_ctl(_epoll, EPOLL_CTL_ADD, TSocketSrv::fd(), &event) == -1)
    {
        std::cerr << "epoll_ctl\n";
        return SocketResult::SR_ERROR;
    }

    epoll_event events[MAXEVENTS];

    std::cout << "Packet Server Listen Loop Entered\n";
    while(_exit == false)
    {
        int n = epoll_wait(_epoll, events, MAXEVENTS, 1000);
        if (n == 0)
        {
            //Maintain Phase
        }
        else
        {
            for (int i = 0; i < n; i++)
            {
    //            std::cout << n << ". epoll_wait released (" << events[i].data.fd << ") : " << EPollEvents(events[i].events) << std::endl;

                // ........
                if (events[i].events & EPOLLERR)
                {
                    if (events[i].data.fd == TSocketSrv::fd())
                        OnErrorServer(SocketResult::SR_ERROR_EPOLL);
                    else
                        OnErrorClient(SocketResult::SR_ERROR_EPOLL);

                    Disconnect(events[i].data.fd);
                }
                // ........
                if (events[i].events & EPOLLIN)
                {
                    if (events[i].data.fd == TSocketSrv::fd())
                        Accept();
                    else
                        Recv(events[i].data.fd);
                }
                // ........
                if (  events[i].events & EPOLLRDHUP  ||
                      events[i].events & EPOLLHUP     )
                {
                    Disconnect(events[i].data.fd);
                }
            }
        }
    }
    std::cout << "Packet Server Listen Loop Quitted\n";

}




template <typename TSocketSrv, typename TSocketClt>
void SocketServer<TSocketSrv, TSocketClt>::Accept()
{
    epoll_event event;
    sockaddr    in_addr;

    socklen_t in_len = sizeof(in_addr);
    int infd;

    while ((infd = ::accept(TSocketSrv::fd(), &in_addr, &in_len)) != -1)
    {
        TSocketClt inSock(infd, "Client");

        if (SocketResult::SR_ERROR == inSock.SetNonBlock())
        {
            std::cerr << "SetNonBlock\n";
            continue; //abort(); ????
        }

        event.data.fd = infd;
        event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR | EPOLLET | EPOLLMSG;
        //event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET | EPOLLONESHOT | EPOLLWAKEUP | EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG /*| EPOLLEXCLUSIVE*/;
        if (::epoll_ctl(epoll(), EPOLL_CTL_ADD, infd, &event) == -1)
        {
            std::cerr << "epoll_ctl\n"; //abort();
        }

        _clientList.set(infd, std::move(inSock));
        OnAccept(inSock, in_addr);

        in_len = sizeof(in_addr);
    }

    if (errno != EAGAIN && errno != EWOULDBLOCK)
    {
        std::cerr << "accept\n";
    }
};


template<typename TSocketSrv, typename TSocketClt>
void SocketServer<TSocketSrv, TSocketClt>::Recv(int fd)
{
    ssize_t                 count;
    const ssize_t           buffLen = 512;
    uint8_t                 buffTmp[buffLen];
    MemStream<std::uint8_t> stream;

    while ((count = ::read(fd, buffTmp, (buffLen * sizeof(std::uint8_t)) - 1 )))
    {
        if (count == -1)
        {
            if (errno == EAGAIN)
            {
                TSocketClt *clt = _clientList.get(fd);
                if (clt)
                    OnRecv(*clt, std::move(stream));
                return;
            }
            OnErrorClient(SocketResult::SR_ERROR_READ);
            break;
        }
        stream.write(buffTmp, count);
    }

    if (stream.Len() > 0)
    {
        TSocketClt *clt = _clientList.get(fd);
        if (clt)
            OnRecv(*clt, std::move(stream));
    }
}


template <typename TSocketSrv, typename TSocketClt>
void SocketServer<TSocketSrv, TSocketClt>::Disconnect(int fd)
{
    TSocketClt sock("NA");
    if (true == _clientList.erase(fd, sock))
    {
        OnDisconnect(sock);
        sock.Release();
    }
}






#endif /* __SOCKET_SERVER_H__ */

