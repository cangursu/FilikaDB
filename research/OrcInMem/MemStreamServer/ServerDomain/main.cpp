/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: can.gursu
 *
 * Created on 16 July 2018
 */


//#include "Socket.h"
//#include "SourceChannel.h"

#include "SocketServerPacket.h"
#include "SocketClientPacket.h"
#include "StreamPacket.h"

#include "LoggerGlobal.h"
#include "Queue.h"
#include "rapidjson/document.h"

#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include <atomic>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>


const char *DEFAULT_CONF_FILE      = "StreamServer.conf";
const char *DEFAULT_SOURCE_SOCKET  = ".sock_domain_log";
const char *DEFAULT_LOG_SOCKET     = ".sock_domain_log";


struct AppArgs
{
    using fs =  std::experimental::filesystem::path;
    fs _confFile;
    fs _sourceChannel;
    fs _logChannel;
};

void PrintWelcome(void)
{
    std::cout << "Medyasoft Memory Stream Server  " << std::endl;
    std::cout << "Version       : " << GIT_VERSION  << std::endl;
    if (!std::string(GIT_VERSION).find(".DEV") != std::string::npos)
    {
        std::cout << "Commit Date   : " << GIT_DATE       << std::endl;
        std::cout << "Build Date    : " << BUILD_DATE     << std::endl;
        std::cout << "Commit Hash   : " << GIT_COMMIT     << std::endl;
    }
    std::cout << std::endl;
}

void PrintUsage(char*fname)
{
    //if (argc < 2)
    {
        std::experimental::filesystem::path p = fname;

        std::cout << std::endl;
        std::cout << "Usage : " << std::endl;
        std::cout << p.filename() << "  -f [--config] configfile -s [--source] sourceChanel -h [-?][--help] " << std::endl;
        std::cout << std::endl;
    }
}

void PrintConfig(const AppArgs &args)
{
    std::cout << "Application Parameters : "                 << std::endl;
    std::cout << "configfile     : " << args._confFile       << std::endl;
    std::cout << "Source Channel : " << args._sourceChannel  << std::endl;
    std::cout << "Log Channel    : " << args._logChannel     << std::endl;
    std::cout << std::endl;
}

rapidjson::ParseResult ConfigLoad(const char *fname, rapidjson::Document &conf)
{
    std::ifstream file(fname, std::ios::in);
    file.seekg(0, std::ios::end);
    int length = file.tellg();

    char buff[length+1];

    file.seekg(0, std::ios::beg);
    file.read(buff, length);
    buff[length +1] = '\0';
    file.close();

    return conf.Parse(buff /*args._confFile.c_str()*/);
}

bool AppArgsValidate(AppArgs &args)
{
    bool result = false;

    if (access( args._confFile.c_str(), F_OK ) == 0)
    {
        result = true;

        rapidjson::Document    conf;
        rapidjson::ParseResult res;

        res = ConfigLoad(args._confFile.c_str(), conf);
        if (res.IsError() == false)
        {
            if (conf.HasMember("StreamServerConfiguration") &&
                conf["StreamServerConfiguration"].IsObject()    )
            {
                rapidjson::Value &obj = conf["StreamServerConfiguration"];

                if (args._sourceChannel.empty())
                {
                    if (obj.HasMember("DomainSocketFile_Source"))
                        args._sourceChannel = obj["DomainSocketFile_Source"].GetString();
                }
                if (args._logChannel.empty())
                {
                    if (obj.HasMember("DomainSocketFile_Log"))
                        args._logChannel = obj["DomainSocketFile_Log"].GetString();
                }
            }
        }
        else
        {
            std::cerr << "Error : " << res.Code() << " - Offset:" << res.Offset() << std::endl;
            std::cerr << "Configuration format error...\n";
            std::cerr << std::endl;
        }
    }

    return result;
}

bool AppArgsProcess(int argc, char** argv, AppArgs &args)
{
    bool result = true;

    const char* const short_opts = "f:s:";
    const option long_opts[] =
    {
            {"config",      required_argument,  nullptr, 'f'},
            {"source",      optional_argument,  nullptr, 's'},
            {"help",        no_argument,        nullptr, 'h'},
            {nullptr,       no_argument,        nullptr, 0}
    };


    int optn = 0;
    while (-1 != (optn = getopt_long(argc, argv, short_opts, long_opts, nullptr)))
    {
        switch (optn)
        {
        case 'f': args._confFile.assign(optarg);        break;
        case 's': args._sourceChannel.assign(optarg);   break;
        case 'l': args._logChannel.assign(optarg);      break;

        case 'h':
        case '?':
        default :
            result = false;
            break;
        }
    }
    if (args._confFile.empty()) args._confFile.assign(DEFAULT_CONF_FILE);

    if (result = AppArgsValidate(args))
    {
        if (args._sourceChannel.empty()) args._sourceChannel.assign(DEFAULT_SOURCE_SOCKET);
        if (args._logChannel.empty())    args._logChannel.assign(DEFAULT_LOG_SOCKET);
    }

    return result;
}

int getch(void)
{
    termios org_opts;
    termios new_opts;

    tcgetattr(STDIN_FILENO, &org_opts);
    new_opts = org_opts;
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);

    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);

    return ch;
}

/*
int getch(void)
{
    termios org_opts;
    termios new_opts;

    int ch   = 0;
    int res = 0;

    //-----  store old settings -----------
    res = tcgetattr(STDIN_FILENO, &org_opts);
    assert(res == 0);

    //---- set new terminal parms --------
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    ch = getchar();

    //------  restore old settings ---------
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);

    return ch;
}
*/





template <typename TQueue>
class PacketConsumer
{
    using byte_t   = StreamPacket::byte_t;
    using msize_t  = StreamPacket::msize_t;

public:
    PacketConsumer(TQueue &que) : _que(que)    {    }
    PacketConsumer(const PacketConsumer &val)   = default;
    PacketConsumer(PacketConsumer &&val)        = default;

    SocketResult    ListenLoop();
    void            ListenLoopStop() { _exit = true; }


private:
    TQueue             &_que;
    std::atomic<bool>   _exit  = false;
};

template <typename TQueue>
SocketResult PacketConsumer<TQueue>::ListenLoop()
{
    std::cout << "Packet Consumer Loop Entered\n";
    while(_exit == false)
    {
        StreamPacket packet;
        if(_que.pop(packet) == true)
        {
            msize_t pkLen = packet.BufferLen();
            msize_t pyLen = packet.PayloadLen();
            LOG_LINE_GLOBAL("SServerClient", " pkLen:", pkLen, ", pyLen:", pyLen);

            const msize_t bufferLen = 128;
            StreamPacket::byte_t buffer [bufferLen];

            for (msize_t i = 0; i < pyLen; i += bufferLen)
            {
                if (packet.PayloadPart(buffer, bufferLen, i) > 0)
                {
                    LOG_LINE_GLOBAL("SServerClient", "Packet:", std::string((char*)buffer, pyLen));
                }
            }
        }
    }
    std::cout << "Packet Consumer Loop Quitted\n";
}




int main(int argc, char** argv)
{
    /*  // Daemonate ....

    pid_t pid = fork();
    if (pid < 0)
    {
        std::cerr << "ERROR : Unable to start Daemon - Fork error\n";
        return EXIT_FAILURE;
    }
    if (pid > 0) return EXIT_SUCCESS;
    umask(0);
    pid_t sid = setsid();
    if (sid < 0)
    {
        std::cerr << "ERROR : Unable to start Daemon - setid error\n";
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0)
    {
        std::cerr << "ERROR : Unable to start Daemon - chdir error\n";
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    */







    AppArgs args;

    PrintWelcome();
    if (false == AppArgsProcess(argc, argv, args))
    {
        PrintUsage(argv[0]);
        PrintConfig(args);
        return -1;
    }
    else
    {
        PrintConfig(args);
    }


    /*
    / *std::thread th = * /SourceChannelServer::listenAsyc( {._sourceName = args._sourceChannel, ._logName = args._logChannel}, true);
    //th.join();
    */
    //std::thread th( (SourceChannelServer({._sourceName = args._sourceChannel, ._logName = args._logChannel})) );



// Packet Queue
    Queue<StreamPacket> queue;

// Consumer Thread
    PacketConsumer<Queue<StreamPacket>> consumer(queue);
    std::thread thcsm([&consumer]() {consumer.ListenLoop();} );



// Reciever Thread
    SocketServerPacket<Queue<StreamPacket>, SocketDomain, SocketClientPacket<SocketDomain>>
            server( { ._sourceName = args._sourceChannel,
                      ._logName    = args._logChannel,
                      ._que        = queue  }                 );
    if (SocketResult::SR_ERROR == server.Init())
    {
        std::cerr << "Unable to initialize Packet Server Server\n";
        return -1;
    }
    std::thread thsrv([&server]() { server.LoopListen(); });



 

    // **********  CLI  **********
    int keycode;
    while ((keycode = getch()) != 'q')
    {
        std::cout << (char)keycode;
    }

    server.LoopListenStop();
    if(thsrv.joinable()) thsrv.join();

    consumer.ListenLoopStop();
    if(thcsm.joinable()) thcsm.join();

    std::cout << "thanks\n";
}

