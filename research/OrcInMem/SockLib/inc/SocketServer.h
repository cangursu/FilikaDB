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
#include "MemStream.h"
#include "SocketUtils.h"

#include <unordered_map>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <atomic>





#define MAXEVENTS 2048






template <typename TSocketSrv, typename TSocketClt>
class SocketServer  : public TSocketSrv
{
    public:
        SocketServer(const char *name) : TSocketSrv(name) {}
        //virtual ~SocketServer() { Release(); }

        SocketResult      Init();
        SocketResult      Release();

        SocketResult      LoopListenPrepare();
        SocketResult      LoopListenSingleShot();
        SocketResult      LoopListen();
        void              LoopListenStop()     { _exit = true; }

        void              Accept();
        virtual void      Recv(int fd);
        void              Disconnect();
        void              Disconnect(int fd);
        void              Disconnect(const TSocketClt &cln) {Disconnect(cln.fd());}
        void              Disconnect(const TSocketClt *cln) {if (cln) Disconnect(cln->fd());}

        std::uint64_t     ClientCount() const        { return _clientList.size();        }
        const TSocketClt *ClientByIdx(int idx) const { return _clientList.getByIdx(idx); } //CAUTION : Beware to use this, CPU abuser...
      //const TSocketClt *ClientByFd (int fd ) const { return _clientList.getByFD(fd);   }

    public:
        // Events
        virtual void  OnAccept      (const TSocketClt &, const sockaddr &)           = 0;
        virtual void  OnRecv        (TSocketClt &,       MemStream<std::uint8_t> &&) = delete;
        virtual void  OnDisconnect  (const TSocketClt &)                             = 0;
        virtual void  OnErrorClient (const TSocketClt &, SocketResult)               = 0;
        virtual void  OnErrorServer (SocketResult)                                   = 0;

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
                TSocketClt* getByFD(int fd)
                {
                    auto it = _map.find(fd);
                    if (it != _map.cend())
                        return &it->second;
                    return nullptr;
                }
                const TSocketClt* getByFD(int fd) const
                {
                    auto it = _map.find(fd);
                    if (it != _map.cend())
                        return &it->second;
                    return nullptr;
                }
                const TSocketClt* getByIdx(std::uint64_t idx) const //CAUTION : Beware to use this, CPU abuser...
                {
                    if (idx > _size) return nullptr;

                    auto it    = _map.cbegin();
                    auto itEnd = _map.cend();
                    int count  = 0;

                    while(it != itEnd && count++ < idx)
                        ++it;

                    return it == itEnd ? nullptr : &(it->second);
                }

                std::uint64_t size () const { return _size; }
            private :
                std::unordered_map<int, TSocketClt> _map;
                std::atomic<std::uint64_t>          _size = 0;
        } _clientList;


        int                 _epoll = -1;
        std::atomic<bool>   _exit  = false;
};


template <typename TSocketSrv, typename TSocketClt>
SocketResult SocketServer<TSocketSrv, TSocketClt>::Init ()
{
    Release();

    SocketResult res = TSocketSrv::InitServer();
    if(SocketResult::SR_SUCCESS != res)
    {
        std::cerr << "ERROR : InitServer\n";
        OnErrorServer(res);
    }


    if (-1 == (_epoll = ::epoll_create1(0)))
    {
        std::cerr << "ERROR : epoll_create1\n";
        OnErrorServer(res = SocketResult::SR_ERROR_EPOLL);
    }


    return res;
}


template <typename TSocketSrv, typename TSocketClt>
SocketResult SocketServer<TSocketSrv, TSocketClt>::Release()
{
    this->Disconnect();

    if (_epoll != -1)
    {
        ::close(_epoll);
        _epoll = -1;
    }
    return TSocketSrv::Release();
}


template <typename TSocketSrv, typename TSocketClt>
SocketResult SocketServer<TSocketSrv, TSocketClt>::LoopListenPrepare()
{
    TSocketSrv::SetNonBlock();

    if (::listen(TSocketSrv::fd(), 1024) == -1)
    {
        std::cerr << "ERROR : Listen\n";
        return SocketResult::SR_ERROR;
    }
    //std::cout << "SocketServer Listenin : " << TSocketSrv::PrmDesc() << std::endl;


    //epoll_event event {.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET | EPOLLONESHOT | EPOLLWAKEUP | EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG /*| EPOLLEXCLUSIVE*/};
    epoll_event event {.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET | EPOLLMSG};
    event.data.fd = TSocketSrv::fd();

    if (::epoll_ctl(_epoll, EPOLL_CTL_ADD, TSocketSrv::fd(), &event) == -1)
    {
        std::cerr << "ERROR : epoll_ctl\n";
        return SocketResult::SR_ERROR;
    }

    return SocketResult::SR_SUCCESS;
}


template <typename TSocketSrv, typename TSocketClt>
SocketResult SocketServer<TSocketSrv, TSocketClt>::LoopListenSingleShot()
{
    SocketResult res = SocketResult::SR_SUCCESS;
    epoll_event events[MAXEVENTS];

    int n = ::epoll_wait(_epoll, events, MAXEVENTS, 1000);
    for (int i = 0; i < n; i++)
    {
        //std::cout << n << ". epoll_wait released (" << events[i].data.fd << ") : " << EPollEvents(events[i].events) << std::endl;

        // .......................................
        // ............... EPOLLERR ..............
        if (events[i].events & EPOLLERR)
        {
            if (events[i].data.fd == TSocketSrv::fd())
                OnErrorServer(res = SocketResult::SR_ERROR_EPOLL);
            else
            {
                const TSocketClt *cln = _clientList.getByFD(TSocketSrv::fd());
                if(cln)
                    OnErrorClient(*cln, res = SocketResult::SR_ERROR_EPOLL);
            }
            Disconnect(events[i].data.fd);
        }

        // .......................................
        // ............... EPOLLIN ...............
        if (events[i].events & EPOLLIN)
        {
            if (events[i].data.fd == TSocketSrv::fd())
                Accept();
            else
                Recv(events[i].data.fd);
        }

        // .......................................
        // ......... EPOLLRDHUP/EPOLLHUP .........
        if (  events[i].events & EPOLLRDHUP  ||
              events[i].events & EPOLLHUP     )
        {
            Disconnect(events[i].data.fd);
        }
    }

    return res;
}


template <typename TSocketSrv, typename TSocketClt>
SocketResult SocketServer<TSocketSrv, TSocketClt>::LoopListen()
{
    SocketResult res = LoopListenPrepare();
    if (SocketResult::SR_SUCCESS == res)
    {
//        std::cout << "Packet Server Listen Loop Entered\n";
        while(_exit == false)
        {
            res = LoopListenSingleShot();
        }
    }
//    std::cout << "Packet Server Listen Loop Quitted\n";

    return res;
}


template <typename TSocketSrv, typename TSocketClt>
void SocketServer<TSocketSrv, TSocketClt>::Accept()
{
    epoll_event event;
    sockaddr    in_addr;

    socklen_t in_len = sizeof(in_addr);
    int infd = -1;

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
        //event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET | EPOLLONESHOT | EPOLLWAKEUP | EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG / *| EPOLLEXCLUSIVE* /;
        if (::epoll_ctl(epoll(), EPOLL_CTL_ADD, infd, &event) == -1)
        {
            OnErrorServer(SocketResult::SR_ERROR_EPOLL);
            //std::cerr << "epoll_ctl\n"; //abort();
        }

        _clientList.set(infd, std::move(inSock));
        OnAccept(*_clientList.getByFD(infd), in_addr);

        in_len = sizeof(in_addr);
    }

    if (errno != EAGAIN && errno != EWOULDBLOCK)
    {
        OnErrorServer(SocketResult::SR_ERROR_ACCEPT);
        //std::cerr << "accept\n";
    }
};


template<typename TSocketSrv, typename TSocketClt>
void SocketServer<TSocketSrv, TSocketClt>::Recv(int fd)
{
    ssize_t                 count            = 0;
    const ssize_t           buffLen          = 512;
    uint8_t                 buffTmp[buffLen] = "";
    MemStream<std::uint8_t> stream;

    TSocketClt *clt = _clientList.getByFD(fd);
    if (nullptr == clt)
    {
        OnErrorServer(SocketResult::SR_ERROR_NOTCLIENT);
        return;
    }

    while (count = clt->Read(buffTmp, (buffLen * sizeof(std::uint8_t)) - 1 ))
    {
        if (count == -1)
        {
            if (errno == EAGAIN)
            {
                clt->OnRecv(std::move(stream));
                return;
            }
            OnErrorClient(*clt,  SocketResult::SR_ERROR_READ);
            break;
        }
        stream.Write(buffTmp, count);
    }

    if (stream.Len() > 0)
    {
        clt->OnRecv(std::move(stream));
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


template <typename TSocketSrv, typename TSocketClt>
void SocketServer<TSocketSrv, TSocketClt>::Disconnect()
{
    while (_clientList.size() > 0)
        Disconnect(_clientList.getByIdx(0));
}



#endif /* __SOCKET_SERVER_H__ */

