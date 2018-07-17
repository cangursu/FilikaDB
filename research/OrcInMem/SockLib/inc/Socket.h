

#ifndef __SOCKET_H__
#define __SOCKET_H__


// *****************************************************
// Server / Client Socket Implementation
//
//


#include "MemStream.h"

//#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <unordered_map>



class Socket
{
    public  :
        Socket(int s = -1);
        Socket(const Socket &s) = delete;
        Socket(Socket &&s);
        ~Socket();

        Socket& operator=(Socket &&s);

        SocketResult Init();
        void         Release();

        SocketResult SetNonBlock();
        SocketResult NameInfo(const sockaddr &addr, std::string &host, std::string &serv) const;

    protected :
        int fd() const { return _fdSock; }
    private :
        int _fdSock = -1;

};


class SocketServer : public Socket
{
    public:
        SocketResult Init();
        SocketResult Release();
        SocketResult ListenLoop();

        void Accept();
        void Recv(int fd);
        void Disconnect(int fd);

        std::uint64_t ClientCount() { return _clientList.size(); }


    public:
        // Events
        virtual void OnAccept      (const Socket &, const sockaddr &)           = 0;
        virtual void OnRecv        (const Socket &, MemStream<std::uint8_t> &&) = 0;
        virtual void OnDisconnect  (const Socket &)                             = 0;
        virtual void OnErrorClient (SocketResult)                               = 0;
        virtual void OnErrorServer (SocketResult)                               = 0;

    protected :
        int epoll() const  { return _epoll; }

    private :
        class FdMap
        {
            public  :
                bool erase(int fd, Socket &s)
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
                bool set(int fd, Socket &&s)
                {
                    _map[fd] = std::move(s);
                    _size++;
                    return true;
                }
                const Socket &get(int fd)
                {
                    auto it = _map.find(fd);
                    if (it != _map.cend()) return it->second;
                    return std::move(Socket());
                }
                std::uint64_t size () { return _size; }
            private :
                std::unordered_map<int, Socket> _map;
                std::uint64_t                   _size = 0;
        } _clientList;


        int   _epoll = -1;
};


class SocketClient : public Socket
{
    public:
        SocketResult Connect (const std::string &host, int port);
        SocketResult Send    (void *data, std::uint64_t len);
        SocketResult Send    (MemStream<std::uint8_t> &&)   ;

    public:
        // Events
        virtual void OnRecv        (const Socket &, MemStream<std::uint8_t> &&) = 0;
        virtual void OnErrorClient (SocketResult)                               = 0;
};


#endif // __SOCKET_H__