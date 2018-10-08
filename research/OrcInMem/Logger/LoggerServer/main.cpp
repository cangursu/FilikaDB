
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
            _flags ^= (-enable ^ _flags.load()) & (1UL << FLAG_POS_DISPLAY);
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


        virtual void OnAccept(TSockCln &sock, const sockaddr &addr)
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

    virtual void OnErrorClient (SocketResult err)
    {
        std::cerr << "Error : LogServerClient::OnErrorClient - " << SocketResultText(err) << std::endl;
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

void Help()
{
    std::cout << std::endl;
    std::cout << "Keybord Functionalities : \n";
    std::cout << "   q -> quit\n";
    std::cout << "   d -> Toogle display log entries\n";
    std::cout << "   h -> help\n";
}

int main(int argc, char** argv)
{
    const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "\n\nFilika Log Server (" << sfile << ")\n";

    LogLineGlbSocketName(nullptr);

    LogServer<SocketDomain, LogServerClient> srv("LogServer");
    srv.SocketPath(sfile);

    if (SocketResult::SR_ERROR == srv.Init())
    {
        std::cerr << "Unable to initialize Echo Server\n";
    }


    Help();


    srv.LoopListenPrepare();
    bool quit = false;
    char ch;

    std::cout << "Packet Server Listen Loop Entered\n";


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

                case '\n':
                case '\r':
                    std::cout << std::endl;
                    break;

                case 'd' :
                case 'D' :
                    g_options.Display(!g_options.Display());
                    std::cout << (g_options.Display() ? "log entries displaying \n" : "log entries hiding \n");
                    break;

                case 'h' :
                case 'H' :
                    Help();
                    break;
            }
        }
    }
    rawmode(false);
    std::cout << "Packet Server Listen Loop Quitted\n";

    return 0;
}



