
#include "OrcRemoteServer.h"
#include "OrcRemoteServerClient.h"

#include "Console.h"

#include <thread>

//#define SOCK_PATH_DEFAULT "/home/postgres/.sock_domain_pgext"
#define SOCK_LOG_DEFAULT  "/home/postgres/.sock_domain_log"



#define KEY_EVENT_ID_HELP 0
#define KEY_EVENT_ID_QUIT 1
#define KEY_EVENT_ID_LF   2


class MyConsole : public Console
{
public:
    MyConsole(OrcRemoteServer<SocketTCP, OrcRemoteServerClient> &server)
        :_server (server)
    {
    }
    virtual void EventFired(const Console::KeyHandler &kh)
    {
        switch(kh._id)
        {
            case KEY_EVENT_ID_HELP  :   DisplayHelp();  break;
            case KEY_EVENT_ID_QUIT  :   _server.LoopListenStop(); LoopStop();  break;
            case KEY_EVENT_ID_LF    :   std::cout   << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl; break;
        }
    }

private :
    OrcRemoteServer<SocketTCP, OrcRemoteServerClient> &_server;
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

    // Prepare Server
    OrcRemoteServer<SocketTCP, OrcRemoteServerClient> srv("OrcRemoteServer");
    srv.Address("127.0.0.1", 5001);



    MyConsole con(srv);
    con.KeyHandlerAdd({'h', 'H'}            , KEY_EVENT_ID_HELP, "h"       , "Help");
    con.KeyHandlerAdd({'q', 'Q', 'x', 'X'}  , KEY_EVENT_ID_QUIT, "q"       , "Quit");
    con.KeyHandlerAdd({'\n', '\r'}          , KEY_EVENT_ID_LF  , "Enter"   , "Line feed");

    //const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    con.DisplayMsg("\nRemote ORC Test SERVER");



    con.DisplayMsg("Server Path : ") << /*sfile*/ srv.Address() << ":" << srv.Port() << std::endl << std::endl;
    con.DisplayHelp();


    if (SocketResult::SR_SUCCESS != srv.Init())
    {
        con.DisplayErrMsg("Unable to initialize Echo Server\n");
        return -1;
    }


    std::thread th( [&srv](){srv.LoopListen(); } );

    con.DisplayMsg("Packet Server Listen Loop Entered\n");
    con.LoopStart();
    if (th.joinable()) th.join();
    con.DisplayMsg("Packet Server Listen Loop Quitted\n");







/*
    con.DisplayMsg("Packet Server Listen Loop Entered\n");

 *     srv.LoopListenPrepare();
    bool quit = false;
    char ch;

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
    */

    return 0;
}



