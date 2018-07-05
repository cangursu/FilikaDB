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
    LSockLog &logger();
};


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

    os.flags(std::ios::left);
    os.width(12);
    os << desc << " ";

    std::stringstream osTmp;
    osTmp << fname << ":" << lineNo;

    os.width(24);
    os << osTmp.str();


    os << " ";
    os.width(20);
    os << funcName << " - ";

    LogLineGlobalFormat(os, args ...);
    //os << std::endl;

    Filika::logger().send();
}


#define LOG_LINE_GLOBAL(desc, ...) LogLineGlobalFormat(desc, __LINE__, __FILENC__, __func__, ## __VA_ARGS__)

#endif // __LOGGER_GLOBAL_H__
