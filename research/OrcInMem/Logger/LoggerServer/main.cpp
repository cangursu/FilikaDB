
#include "SocketClientPacket.h"
#include "SocketServer.h"


#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SOCK_PATH_DEFAULT "/home/postgres/.sock_domain_log"


class LogServerOptions
{
    public  :
        ~LogServerOptions()                        = default;
        LogServerOptions(const LogServerOptions &) = delete;
        LogServerOptions(LogServerOptions &&)      = delete;
        LogServerOptions()
        {
            Display(true);
        }

        void Display(bool enable)
        {
//            number ^= (-x ^ number) & (1UL << n);
            _flags ^= (-enable ^ _flags.load()) & (1UL << FLAG_POS_DISPLAY);
/*
            std::int16_t f = _flags.load();
            if (enable)
                _flags = f | (1UL << FLAG_POS_DISPLAY);
            else
                _flags = f & ~(1UL << FLAG_POS_DISPLAY);
*/
        }
        bool Display()
        {
            return (_flags.load() >> FLAG_POS_DISPLAY) & 1U;
        }

    private :
        std::atomic<std::int16_t> _flags = 0L;

        static const int FLAG_POS_DISPLAY = 1;

} g_options;


template <typename TSockSrv, typename TSockCln>
class LogServer : public SocketServer<TSockSrv, TSockCln>
{
    using  msize_t = StreamPacket::msize_t;

    public:

        LogServer(const char *name):SocketServer<TSockSrv, TSockCln>(name)
        {
        }


        virtual void OnAccept(const TSockCln &sock, const sockaddr &addr)
        {
            std::cout << "Accepted connection - ";
            ClientCount();
        }
/*
        virtual void OnRecv(TSockCln &sock, MemStream<std::uint8_t> &&stream)
        {
            //std::cout << stream.dump("LogServer::OnRecv");
            sock.OnRecv(std::move(stream));
        }
*/
        virtual void OnDisconnect  (const TSockCln &sock)
        {
            std::cout << "Client Disconnected - ";
            ClientCount();
        }

        virtual void OnErrorClient(const TSockCln &, SocketResult res)
        {
            std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }

        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };

        void ClientCount()
        {
            std::cout << "Client count = " << SocketServer<TSockSrv, TSockCln>::ClientCount() << std::endl;
        }
};


class LogServerClient : public SocketClientPacket<SocketDomain>
{
public:
    LogServerClient(const char *name = "LogServerClient")
            : SocketClientPacket(name)
    {
    }

    LogServerClient(int fd, const char *name)
        : SocketClientPacket(fd, name)
    {
    }

    virtual void OnRecvPacket(StreamPacket &&packet)
    {
        if (g_options.Display())
            Display(packet);
    }

    void Display(StreamPacket &packet)
    {
        const msize_t        buffLen = 512;
        StreamPacket::byte_t buff [buffLen];

        msize_t pyLenPart = 0;
        msize_t pyLen = packet.PayloadLen();
        for (msize_t offset = 0; offset < pyLen; offset += buffLen)
        {
            if ( (pyLenPart = packet.PayloadPart(buff, buffLen, offset)) > 0)
                std::cout << std::string((char*)buff, pyLenPart);
        }
        std::cout << std::endl;
    }
};






void rawmode(bool enable)
{
    termios term;
    tcgetattr(0, &term);
    if (enable)
        term.c_lflag &= ~(ICANON | ECHO);
    else
        term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}


bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

int main(int argc, char** argv)
{
    const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "Filika Logger Receiver " << std::endl;

    LogLineGlbSocketName(nullptr);

    LogServer<SocketDomain, LogServerClient> srv("LogServer");
    srv.SocketPath(sfile);

    if (SocketResult::SR_ERROR == srv.Init())
    {
        std::cerr << "Unable to initialize Echo Server\n";
    }


    srv.LoopListenPrepare();
    bool quit = false;
    char ch;

    std::cout << "Packet Server Listen Loop Entered\n";
    std::cout << "Keybord Functionalities : \n";
    std::cout << "   q -> quit\n";
    std::cout << "   d -> display log entries\n";
    std::cout << "   h -> hide log entries\n";

    rawmode(true);
    while(false == quit)
    {
        srv.LoopListenSingleShot();
        if (kbhit())
        {
            read(STDIN_FILENO, &ch, 1);
            switch(ch)
            {
                case 'q' :
                case 'Q' :
                case 'x' :
                case 'X' :
                    quit = true;
                    break;

                case 'd' :
                case 'D' :
                    std::cout << "log entries displaying \n";
                    g_options.Display(true);
                    break;

                case 'h' :
                case 'H' :
                    std::cout << "log entries hiding \n";
                    g_options.Display(false);
                    break;
            }
        }
    }
    rawmode(false);
    std::cout << "Packet Server Listen Loop Quitted\n";

    return 0;
}



