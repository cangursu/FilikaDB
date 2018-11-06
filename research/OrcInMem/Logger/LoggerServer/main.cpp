
#include "SocketClientPacket.h"
#include "SocketServer.h"
#include "Console.h"


#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <thread>

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


#define KEY_EVENT_ID_HELP           0
#define KEY_EVENT_ID_QUIT           1
#define KEY_EVENT_ID_LF             2
#define KEY_EVENT_ID_OPT_DISPLAY    3

template <typename TServer>
class LogServerConsole : public Console
{
public:
    TServer &_srv;
    LogServerConsole (TServer &srv) : _srv(srv)
    {
    }

    virtual void EventFired(const Console::KeyHandler &kh)
    {
        switch(kh._id)
        {
            case KEY_EVENT_ID_HELP          :   DisplayHelp();  break;
            case KEY_EVENT_ID_QUIT          :   _srv.LoopListenStop(); LoopStop();  break;
            case KEY_EVENT_ID_LF            :   for(int i = 0; i < 24; i++) std::cout << std::endl; break;
            case KEY_EVENT_ID_OPT_DISPLAY   :   g_options.Display(!g_options.Display());
                                                DisplayMsg(g_options.Display() ? "log entries displaying \n" : "log entries hiding \n");
                                                break;
        }
    }
};




int main(int argc, char** argv)
{
    const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    //std::cout << "\n\nFilika Log Server (" << sfile << ")\n";

    LogLineGlbSocketName(nullptr);

    LogServer<SocketDomain, LogServerClient> srv("LogServer");
    srv.SocketPath(sfile);

    if (SocketResult::SR_ERROR == srv.Init())
    {
        std::cerr << "Unable to initialize Echo Server\n";
    }




    LogServerConsole<LogServer<SocketDomain, LogServerClient>> con(srv);
    con.KeyHandlerAdd({'h', 'H'}            , KEY_EVENT_ID_HELP       , "h"       , "Help");
    con.KeyHandlerAdd({'q', 'Q', 'x', 'X'}  , KEY_EVENT_ID_QUIT       , "q"       , "Quit");
    con.KeyHandlerAdd({'\n', '\r'}          , KEY_EVENT_ID_LF         , "Enter"   , "Line feed");
    con.KeyHandlerAdd({'d', 'D'}            , KEY_EVENT_ID_OPT_DISPLAY, "d"       , "Toogle Display Messages");

    con.DisplayMsg("\nCIMRI Central Log Server ver:1.0");
    con.DisplayMsg("Path : " + srv.SocketPath() + "\n");
    con.DisplayHelp();

    con.DisplayMsg("CIMRI Central Log Server Listen Loop Entered\n");
    std::thread thSrv ([&srv] () { srv.LoopListen();} );
    con.LoopStart();
    if (thSrv.joinable()) {thSrv.join();}
    con.DisplayMsg("CIMRI Central Log Server Listen Loop Quitted\n");


    return 0;
}



