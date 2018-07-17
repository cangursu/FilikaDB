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


#include "Socket.h"
#include "SocketDomain.h"
#include "rapidjson/document.h"


#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <getopt.h>


class MemStreamReqReciever : public SockDomain
{

};

struct AppArgs
{
    using fs =  std::experimental::filesystem::path;
    fs _confFile;
    fs _sourceChannel;
};


void PrintWelcome(void)
{
    std::cout << "Medyasoft Memory Stream Server  " << std::endl;
    std::cout << "Version       : " << GIT_VERSION    << std::endl;
    if (!std::string(GIT_VERSION).find(".DEV") != std::string::npos)
    {
        std::cout << "Commit Date   : " << GIT_DATE       << std::endl;
        std::cout << "Build Date    : " << BUILD_DATE     << std::endl;
        std::cout << "Commit Hash   : " << GIT_COMMIT     << std::endl;
    }
}

void PrintUsage(char*fname)
{
    //if (argc < 2)
    {
        std::experimental::filesystem::path p = fname;

        std::cout << std::endl;
        std::cout << "Usage : " << std::endl;
        std::cout << p.filename() << "  -f [--config] configfile -s [--source] sourceChanel -h [-?][--help] " << std::endl;
    }
}

void PrintConfig(const AppArgs &args)
{
    std::cout << "configfile    : " << args._confFile       << std::endl;
    std::cout << "sourceChannel : " << args._sourceChannel  << std::endl;
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


bool AppArgsValidate(int argc, char** argv, AppArgs &args)
{
    bool result = false;

    if (access( args._confFile.c_str(), F_OK ) == 0)
    {
        result = true;

        rapidjson::Document conf;
        if (ConfigLoad(args._confFile.c_str(), conf))
        {
            if (conf["StreamServerConfiguration"].IsObject())
            {
                rapidjson::Value &obj = conf["StreamServerConfiguration"];

                if (args._sourceChannel.empty())
                {
                    if (obj.HasMember("SourceDomainSocketFile"))
                        args._sourceChannel = obj["SourceDomainSocketFile"].GetString();
                }
            }
        }
        else
        {
            //std::cerr << GetParseError_En(ok.Code()) << " - Line:" << ok.Offset();
            std::cerr << "Config format error...\n";
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

        case 'h':
        case '?':
        default :
            result = false;
            break;
        }
    }

    return result ? AppArgsValidate(argc, argv, args) : result;
}

class SourceChannel : public SockDomain
{
public:
    SourceChannel(const char *sockName) : SockDomain(sockName)
    {

    }


    SocketResult recv(std::string &item)
    {
        static __thread  char buff[1024];

        SocketResult res = SocketResult::ERROR;
        int len = recvFrom(buff, 1024);
        if (len > 0 && len < 1024)
        {
            res = SocketResult::SUCCESS;
            item.assign(buff, len);
        }
        return res;
    }


};

int main(int argc, char** argv)
{
    AppArgs args;

    PrintWelcome();
    if (false  == AppArgsProcess(argc, argv, args))
    {
        PrintUsage(argv[0]);
        return -1;
    }
    else
    {
        PrintConfig(args);
    }

/*
    SourceChannel src(args._sourceChannel.c_str());
    std::string logitem;
    while (true)
    {
        src.recv(logitem);
        std::cout << logitem << std::endl;
    }
*/
}
