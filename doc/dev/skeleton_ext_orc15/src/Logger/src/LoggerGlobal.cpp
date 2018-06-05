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
    
    
    LSockLog &logger()
    {
        static LSockLog _glog (sfile);
        return _glog;
    }
    
    inline void LF(char *p)
    {
        *p++ = '\n';
        *p++ = '\0';    
    }
    
    EXTERN_C void LogLineGlobalFormat(const char *desc, int lineNo, const char *fname, const char *funcName, const char *fmt,...)
    {
        std::va_list list;
        va_start(list, fmt);

        char prbf[1024] = "";
        size_t len = std::vsnprintf(prbf, 1024, fmt, list);
        LF(prbf + len);
        
        logger().Get(LSL_INFO) << lineNo << ":" << fname << ":" << funcName << ":" << desc << " - " << prbf;
        logger().send();
        //LogLine(logger(), desc, fmt, lineNo, fname, funcName, list);
    }
    
    
    int LogLineGlobal(const char *desc, int count, ...)
    {
        va_list args;
        va_start(args, count);
        for (int i = 0; i < count; ++i) 
        {
            va_arg(args, int);
        }
        
        return 0;
    }
    
    

    /*
    EXTERN_C void LogLineGlb_3(const char *desc, const char *fmt, int lineNo, const char *fname, const char *funcName, ...)
    {
        std::va_list list;
        va_start(list, funcName);
        
        LogLine(logger(), desc, fmt, lineNo, fname, funcName, list);
    }
    
    
    EXTERN_C void LogLineGlb_2(const char *desc, const char *fmt, const char *funcName, ...)
    {
        std::va_list list;
        va_start(list, funcName);
        
        LogLine(logger(), desc, fmt, funcName, list);
    }
    
    
    EXTERN_C void LogLineGlb_1(const char *fmt, ...)
    {
        std::va_list list;
        va_start(list, fmt);
        
        LogLine(logger(), fmt, list);    
    }
    */
    
    /*
    EXTERN_C void LogLineGlobal(const char *fmt, ...)
    {
        logger().send( static_cast<std::stringstream &>(std::stringstream()
                                                                    << std::to_string(__LINE__) 
                                                                    << std::string(":") 
                                                                    << __FILEN__ 
                                                                    << std::string(" -> ") 
                                                                    << __func__ 
                                                                    << " - " 
                                                                    << std::string(msg)).str() );
    }
    */
}