/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   LoggerGlobal.h
 * Author: can.gursu@medyasoft.com.tr
 *
 * Created on April 19, 2018, 9:44 AM
 */

#ifndef __LOGGER_GLOBAL_H__
#define __LOGGER_GLOBAL_H__


#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif



#define SOCK_PATH_DEFAULT "unix_sock.server"
#define __FILENC__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#include "Logger.h"
namespace  Filika
{
    const char *SocketFile();
    LSockLog &logger();
};


#include <cstring>
#include <sstream>


EXTERN_C void   LogLineGlbSocketName(const char *path);
inline void     LogLineGlobalFormat(std::ostringstream &)
{
}

template<typename T, typename ... Args>
void LogLineGlobalFormat(std::ostringstream &os, T t,  Args ... args)
{
    os << t;
    LogLineGlobalFormat(os, args ...);
}

template<typename ... Args>
void LogLineGlobalFormat(const char *desc, int lineNo, const char *fname, const char *funcName, Args ... args)
{
    std::ostringstream &os = Filika::logger().Get(Filika::LSL_INFO);

    //Filika::LSockLog logger(Filika::SocketFile());
    //std::ostringstream &os = logger.Get(Filika::LSL_INFO);

    const int widthDesc  = 12;
    const int widthFName = 24;
    const int widthFunc  = 20;

    os.flags(std::ios::left);
    os.width(widthDesc);
    os << desc << " ";

    std::stringstream osTmp;
    std::string       strTmp = ":" + std::to_string(lineNo);
    int               widthFNamePart = std::strlen(fname) < widthFName - strTmp.size() ? std::strlen(fname) : widthFName - strTmp.size();
    osTmp << std::string(fname, widthFNamePart) << strTmp;

    os.width(widthFName);
    os << osTmp.str();

    os << " ";
    os.width(widthFunc);
    os << std::string(funcName, std::strlen(funcName) < widthFunc ? std::strlen(funcName) : widthFunc) << " - ";

    LogLineGlobalFormat(os, args ...);

    //logger.send();
    if (Filika::FILIKA_RESULT::FR_ERROR == Filika::logger().send())
    {
        std::cerr << "ERROR - Log : Unable to send log\n";
    }
}


#define LOG_LINE_GLOBAL(desc, ...) LogLineGlobalFormat(desc, __LINE__, __FILENC__, __func__, ## __VA_ARGS__)

#endif // __LOGGER_GLOBAL_H__
