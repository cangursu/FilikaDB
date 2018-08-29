/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "LoggerGlobal.h"
#include "Logger.h"

namespace  Filika
{
    static const char *sfile = SOCK_PATH_DEFAULT;

    EXTERN_C void LogLineGlbSocketName(const char *path)
    {
        sfile = path;
    }

    const char *SocketFile() {return sfile;}

    LSockLog &logger()
    {
        //std::cout << "logger               --> " << sfile << std::endl;
        static LSockLog _glog (sfile);
        return _glog;
    }

    inline void LF(char *p)
    {
        *p++ = '\n';
        *p++ = '\0';
    }
}