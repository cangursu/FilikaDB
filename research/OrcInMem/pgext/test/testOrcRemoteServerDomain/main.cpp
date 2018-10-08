
#include "SocketClientPacket.h"
#include "SocketTCP.h"
#include "SocketServer.h"
#include "Console.h"
#include "MemStreamPacketStore.h"
#include "MemStreamPacket.h"


#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

//#define SOCK_PATH_DEFAULT "/home/postgres/.sock_domain_pgext"
#define SOCK_LOG_DEFAULT  "/home/postgres/.sock_domain_log"


template <typename TSockSrv, typename TSockCln>
class OrcRemoteServer : public SocketServer<TSockSrv, TSockCln>
{
    using  msize_t = StreamPacket::msize_t;

    public:

        OrcRemoteServer(const char *name):SocketServer<TSockSrv, TSockCln>(name)
        {
        }


        virtual void OnAccept(TSockCln &sock, const sockaddr &addr)
        {
            sock.SetStore(&_storeInstant);
            std::cout << "Accepted connection - ";
            ClientCount();
        }
/*
        virtual void OnRecv(TSockCln &sock, MemStream<std::uint8_t> &&stream)
        {
            //std::cout << stream.dump("OrcRemoteServer::OnRecv");
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

    private:
        MemStreamPacketMap _storeInstant;
};


class OrcRemoteServerClient : public SocketClientPacket<SocketTCP>
{
public:
    OrcRemoteServerClient(const char *name = "OrcRemoteServerClient")
            : SocketClientPacket(name)
    {
    }

    OrcRemoteServerClient(int fd, const char *name)
        : SocketClientPacket(fd, name)
    {
    }

    virtual void OnErrorClient (SocketResult err)
    {
        std::cerr << "Error : OrcRemoteServerClient::OnErrorClient - " << SocketResultText(err) << std::endl;
    }

    virtual void OnRecvPacket(StreamPacket &&packet)
    {
       // if (g_options.Display())
        {
            std::cout << "OnRecvPacket --->\n";
            //DisplayPacket(packet);
        }
        EncodePacket(packet);
        SendPacket(packet);
    }

    void EncodePacket(StreamPacket &packet)
    {
        if (_pstore)
        {
            MemStreamPacket stream;

            std::vector <MemStreamPacket::Cmd> cmds;
            if ( 1 > stream.EncodePacket(packet, cmds))
            {
                OnErrorClient(SocketResult::SR_ERROR_PARSE);
            }
            else
            {
                for(const auto &cmd : cmds)
                {
                    std::cout << "Command \n";
                    std::cout << "_cmdid          : " << cmd._cmdidStr       << std::endl;
                    std::cout << "_source         : " << cmd._source         << std::endl;
                    std::cout << "_sender         : " << cmd._sender         << std::endl;
                    std::cout << "_ts             : " << cmd._ts             << std::endl;
                    std::cout << "_refid          : " << cmd._refid          << std::endl;
                    std::cout << "_result         : " << cmd._result         << std::endl;
                    std::cout << "_message        : " << cmd._message        << std::endl;
                    std::cout << "_buffer._length : " << cmd._buffer._length << std::endl;
                    std::cout << "_buffer._offset : " << cmd._buffer._offset << std::endl;
                    std::cout << "_buffer._enc    : " << cmd._buffer._enc    << std::endl;
                    std::cout << "_buffer._buf    : " << std::string(&cmd._buffer._buf[0], cmd._buffer._buf.size()) << std::endl;


                    switch (cmd._cmdid)
                    {
                        case MemStreamPacket::CmdID::CMD_WRITE   :
                            _pstore->Write(cmd._source.c_str(), &cmd._buffer._buf[0], cmd._buffer._buf.size());
                            break;

                        case MemStreamPacket::CmdID::CMD_LENGTH  :
                        case MemStreamPacket::CmdID::CMD_READ    :
                        default                 :
                            break;
                    }
                }
            }
        }
    }

    void DisplayPacket(StreamPacket &packet)
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

    void SetStore(MemStreamPacketMap *store)
    {
        _pstore = store;
    }
    MemStreamPacketMap *_pstore = nullptr;
};





// *************************************************************************
// ***                                                                   ***
// ***                   APP                                             ***
// ***                                                                   ***
// ***                                                                   ***
// *************************************************************************

int main(int argc, char** argv)
{
    LogLineGlbSocketName(nullptr);

    Console con;

    con.AddKeyHandler({'q', 'Q', 'x', 'X'}  , "q"       , "Quit");
    con.AddKeyHandler({'\n', '\r'}          , "Enter"   , "Line feed");
    con.AddKeyHandler({'h', 'H'}            , "h"       , "Help");

    //const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    con.DisplayMsg("\nRemote ORC Test SERVER\n");


    // Prepare Server
    OrcRemoteServer<SocketTCP, OrcRemoteServerClient> srv("OrcRemoteServer");
    //srv.SocketPath(sfile);
    srv.Address("127.0.0.1", 5001);

    if (SocketResult::SR_ERROR == srv.Init())
    {
        con.DisplayErrMsg("Unable to initialize Echo Server\n");
        return -1;
    }


    con.DisplayMsg("Server Path : ") << /*sfile*/ srv.Address() << ":" << srv.Port() << std::endl << std::endl;
    con.DisplayHelp();


    srv.LoopListenPrepare();
    bool quit = false;
    char ch;

    con.DisplayMsg("Packet Server Listen Loop Entered\n");




/*
    con.rawmode(true);
    Loop loop([&con]()->void
    {

    } );
    con.rawmode(false);
*/



    con.rawmode(true);
    while(false == quit)
    {
        //con.DisplayMsg("x");
        srv.LoopListenSingleShot();
        if (con.kbhit())
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
                    std::cout << std::endl << std::endl << std::endl
                              << std::endl << std::endl << std::endl;
                    break;

                case 'd' :
                case 'D' :
                    //g_options.Display(!g_options.Display());
                    //std::cout << (g_options.Display() ? "log entries displaying \n" : "log entries hiding \n");
                    break;

                case 'h' :
                case 'H' :
                    con.DisplayHelp();
                    break;
            }
        }
    }
    con.rawmode(false);






    std::cout << "Packet Server Listen Loop Quitted\n";

    return 0;
}



