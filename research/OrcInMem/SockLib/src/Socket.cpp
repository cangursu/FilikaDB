

#include "Socket.h"
#include "SocketUtils.h"


#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/epoll.h>



#define MAXEVENTS 1024




Socket::Socket(int s /*= -1*/)
    : _fdSock(s)
{
}

Socket::Socket(Socket &&s)
    : _fdSock (s._fdSock)
{
    s._fdSock = -1;
}

Socket::~Socket()
{
    Release();
}

Socket& Socket::operator=(Socket &&s)
{
    _fdSock = s._fdSock;
    s._fdSock = -1;
    return *this;
}

SocketResult Socket::Init ()
{
    Release();

    SocketResult res = SocketResult::SUCCESS;
    if ((_fdSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        res = SocketResult::ERROR;
    }

    return res;
}

void Socket::Release()
{
    SocketResult res = SocketResult::SUCCESS;
    if (-1 != _fdSock)
    {
        if (-1 == close(_fdSock)) res = SocketResult::ERROR;
        _fdSock = -1;
    }
}

SocketResult Socket::SetNonBlock()
{
    SocketResult res = SocketResult::ERROR;

    int flags = fcntl(_fdSock, F_GETFL, 0);
    if (-1 == flags)
    {
        perror("fcntl");
        return SocketResult::ERROR;
    }

    flags |= O_NONBLOCK;
    if (-1 == fcntl(_fdSock, F_SETFL, flags))
    {
        perror("fcntl");
        return SocketResult::ERROR;
    }

    return SocketResult::SUCCESS;
}

SocketResult Socket::NameInfo(const sockaddr &addr, std::string &host, std::string &serv) const
{
    SocketResult res = SocketResult::ERROR;

    socklen_t in_len = sizeof(addr);
    char bufHost[NI_MAXHOST];
    char bufServ[NI_MAXSERV];

    if (getnameinfo(&addr, in_len,
                    bufHost, NI_MAXHOST,
                    bufServ, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICHOST) == 0)
    {
        res = SocketResult::SUCCESS;
        host = bufHost;
        serv = bufServ;
    }

    return res;
}






SocketResult SocketServer::Init ()
{
    Release();
    SocketResult res = Socket::Init();

    int opt = 1;
    if (setsockopt(fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    {
        perror("Setsockopt");
        res = SocketResult::ERROR;
    }
    else
    {
        sockaddr_in server_addr {.sin_family = AF_INET,
                                 .sin_port   = htons(5000) };
        server_addr.sin_addr.s_addr = INADDR_ANY;
        std::memset(&(server_addr.sin_zero), 0, 8);

        if (bind(fd(), (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
        {
            OnErrorServer(res = SocketResult::ERROR_BIND);
            perror("Unable to bind");
        }
    }

    return res;
}

SocketResult SocketServer::Release()
{
    if (_epoll != -1)
    {
        close(_epoll);
        _epoll = -1;
    }
    Socket::Release();
}

SocketResult SocketServer::ListenLoop()
{
    SetNonBlock();

    if (listen(fd(), 1024) == -1)
    {
        perror("Listen");
        return SocketResult::ERROR;
    }

    printf("\nTCPServer Waiting for client on port 5000\n");
    fflush(stdout);

    if (-1 == (_epoll = epoll_create1(0)))
    {
        perror("epoll_create1");
        return SocketResult::ERROR;
    }

    //epoll_event event {.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET | EPOLLONESHOT | EPOLLWAKEUP | EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG /*| EPOLLEXCLUSIVE*/};
    epoll_event event {.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET | EPOLLMSG};
    event.data.fd = fd();

    if (epoll_ctl(_epoll, EPOLL_CTL_ADD, fd(), &event) == -1)
    {
        perror("epoll_ctl");
        return SocketResult::ERROR;
    }

    epoll_event events[MAXEVENTS];

    while(true)
    {
        int n = epoll_wait(_epoll, events, MAXEVENTS, 1000);
        for (int i = 0; i < n; i++)
        {
            //std::cout << n << ". epoll_wait released (" << events[i].data.fd << ") : " << EPollEvents(events[i].events) << std::endl;

            // ........
            if (  events[i].events & EPOLLERR)
            {
                if (events[i].data.fd == fd())
                    OnErrorServer(SocketResult::ERROR_EPOLL);
                else
                    OnErrorClient(SocketResult::ERROR_EPOLL);

                Disconnect(events[i].data.fd);
            }
            // ........
            if (events[i].events & EPOLLIN)
            {
                if (events[i].data.fd == fd())
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


void SocketServer::Accept()
{
    epoll_event event;
    sockaddr    in_addr;

    socklen_t in_len = sizeof(in_addr);
    int infd;

    while ((infd = accept(fd(), &in_addr, &in_len)) != -1)
    {
        Socket inSock(infd);

        if (SocketResult::ERROR == inSock.SetNonBlock())
        {
            perror("SetNonBlock");
            continue; //abort(); ????
        }

        event.data.fd = infd;
        event.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR | EPOLLET | EPOLLMSG;
        //event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET | EPOLLONESHOT | EPOLLWAKEUP | EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG /*| EPOLLEXCLUSIVE*/;
        if (epoll_ctl(epoll(), EPOLL_CTL_ADD, infd, &event) == -1)
        {
            perror("epoll_ctl"); //abort();
        }

        _clientList.set(infd, std::move(inSock));
        OnAccept(inSock, in_addr);

        in_len = sizeof(in_addr);
    }

    if (errno != EAGAIN && errno != EWOULDBLOCK)
    {
        perror("accept");
    }
};

void SocketServer::Recv(int fd)
{
    ssize_t                 count;
    const ssize_t           buffLen = 512;
    uint8_t                 buffTmp[buffLen];
    MemStream<std::uint8_t> stream;

    while ((count = read(fd, buffTmp, (buffLen * sizeof(std::uint8_t)) - 1 )))
    {
        if (count == -1)
        {
            if (errno == EAGAIN)
            {
                OnRecv(_clientList.get(fd), std::move(stream));
                return;
            }

            OnErrorClient(SocketResult::ERROR_READ);
            break;
        }

        stream.write(buffTmp, count);
    }
}

void SocketServer::Disconnect(int fd)
{
    Socket sock;
    if (true == _clientList.erase(fd, sock))
    {
        OnDisconnect(sock);
        sock.Release();
    }
}







SocketResult SocketClient::Connect(const std::string &address, int port)
{
    SocketResult result = SocketResult::SUCCESS;
    hostent *host = gethostbyname(address.c_str());

    sockaddr_in addr    {   .sin_family = AF_INET,
                            .sin_port = htons(port),
                            .sin_addr = *((struct in_addr *)host->h_addr)
                        };
    std::memset(&(addr.sin_zero), 0, 8);

    if (connect(fd(), (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
        OnErrorClient(result = SocketResult::ERROR_CONNECT);

    return result;
}

SocketResult SocketClient::Send(void *data, std::uint64_t len)
{
    SocketResult res = SocketResult::SUCCESS;
    if (send(fd(), data, len, 0) == -1)
    {
        res = SocketResult::ERROR_SEND;
        OnErrorClient(res);
    }
    return res;
}

SocketResult SocketClient::Send(MemStream<std::uint8_t> &&stream) 
{
    SocketResult result = SocketResult::SUCCESS;

    const std::uint64_t lenBuff = 512L;
    std::uint64_t       len     = stream.Len();
    std::uint64_t       readed  = 0L;

    char buff[lenBuff] = "";
    for (int offset = 0; offset < len && (result == SocketResult::SUCCESS); offset += readed)
    {
        readed = stream.read(buff, lenBuff, offset);
        result = Send(static_cast<void *>(buff), readed);
    }

    return result;
}









