/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Logger.h
 * Author: postgres
 *
 * Created on April 19, 2018, 9:44 AM
 */


    /*
     * http://www.drdobbs.com/cpp/logging-in-c/201804215
     * https://code.i-harness.com/en/q/7cf18
     * 
     * 
    */


#ifndef __LOGGER_H__
#define __LOGGER_H__


#include <sys/socket.h>
#include <sys/un.h>

#include <iostream>
#include <string>
#include <sstream>
#include <cstdarg>




#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   if defined (BUILDING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllexport)
#   elif defined (USING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllimport)
#   else
#       define FILELOG_DECLSPEC
#   endif // BUILDING_DBSIMPLE_DLL
#else
#   define FILELOG_DECLSPEC
#endif // _WIN32





#define SOCK_PATH_DEFAULT "unix_sock.server"


namespace  Filika
{    
    enum FILIKA_RESULT
    {
        FR_SUCCESS =  0,
        FR_ERROR   = -1
    };
    
    class SockDomain 
    {
        public:

        SockDomain() {} 
        SockDomain(SockDomain &)  = delete;
        SockDomain(SockDomain &&) = delete;
        SockDomain(const char *path); 
        virtual ~SockDomain() ;

        int init();
        int init(const char *path);
        int initServer();
        int release() ;

        ssize_t recvFrom(void *pdata, size_t len) ;
        ssize_t sendTo(const void *pdata, size_t len);

    private:
        int _sock = -1;
        sockaddr_un _addr{AF_UNIX, SOCK_PATH_DEFAULT};
    };



    
    
    
    
#ifndef __LOG_H__
#define __LOG_H__

#include <sstream>
#include <string>
#include <stdio.h>


inline std::string NowTime();

enum TLogStreamLevel
{
    LSL_ERROR, 
    LSL_WARNING, 
    LSL_INFO, 
    LSL_DEBUG, 
    LSL_DEBUG1, 
    LSL_DEBUG2, 
    LSL_DEBUG3, 
    LSL_DEBUG4
};


class LogStream
{
public:
    LogStream() {}
    inline virtual ~LogStream();
    inline std::ostringstream&     Get(TLogStreamLevel level = LSL_INFO);
    inline std::ostringstream&     Get(const char *line, const char *file, TLogStreamLevel level = LSL_INFO);
    inline std::ostringstream&     Get(int line,         const char *file, TLogStreamLevel level = LSL_INFO);
    
    inline void                    Reset() { _os.str(std::move(std::string())); _os.clear(); } 

public:
    inline static TLogStreamLevel& LogLevel();

    inline static std::string      ToString(TLogStreamLevel level);
    inline static TLogStreamLevel  FromString(const std::string& level);

protected:
    std::ostringstream _os;

private:
    LogStream(const LogStream&) = delete;
    LogStream& operator =(const LogStream&) = delete;
};

inline std::ostringstream& LogStream::Get(TLogStreamLevel level /*= LSL_INFO*/)
{    
    _os << NowTime() << " " << ToString(level) << " - ";
    //_os << std::string(level > LSL_DEBUG ? level - LSL_DEBUG : 0, '\t');
    
    return _os;
}

inline std::ostringstream& LogStream::Get(int line, const char *file, TLogStreamLevel level /*= LSL_INFO*/)
{
    auto &o = Get(level);
    o << line << ":" << file << " - ";
    return o;
}

inline std::ostringstream& LogStream::Get(const char *line, const char *file, TLogStreamLevel level /*= LSL_INFO*/)
{
    auto &o = Get(level);
    o << line << ":" << file << " - ";
    return o;
}

inline LogStream::~LogStream()
{
    //_os << std::endl;
    //T::Output(os.str());
}

inline TLogStreamLevel& LogStream::LogLevel()
{
    static TLogStreamLevel logLevel = LSL_DEBUG4;
    return logLevel;
}

inline 
std::string LogStream::ToString(TLogStreamLevel level)
{
    static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4"};
    return buffer[level];
}

inline TLogStreamLevel LogStream::FromString(const std::string& level)
{
    if (level == "DEBUG4")  return LSL_DEBUG4;
    if (level == "DEBUG3")  return LSL_DEBUG3;
    if (level == "DEBUG2")  return LSL_DEBUG2;
    if (level == "DEBUG1")  return LSL_DEBUG1;
    if (level == "DEBUG")   return LSL_DEBUG;
    if (level == "INFO")    return LSL_INFO;
    if (level == "WARNING") return LSL_WARNING;
    if (level == "ERROR")   return LSL_ERROR;
    
    LogStream().Get(LSL_WARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return LSL_INFO;
}




class Output2FILE
{
public:
    static FILE*& Stream();
    static void Output(const std::string& msg);
};

inline FILE*& Output2FILE::Stream()
{
    static FILE* pStream = stderr;
    return pStream;
}

inline void Output2FILE::Output(const std::string& msg)
{   
    FILE* pStream = Stream();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
}



/*
class FILELOG_DECLSPEC FILELog : public LogStream<Output2FILE> {};
//typedef Log<Output2FILE> FILELog;

#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL Filika::TLogStreamLevel::LSL_DEBUG4
#endif

#define FILE_LOG(level) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if (level > Filika::FILELog::LogLevel() || !Filika::Output2FILE::Stream()) ; \
    else Filika::FILELog().Get(level)
*/


class FILELOG_DECLSPEC LSockLogServer : public LogStream,
                                        public SockDomain
{
    public :
        LSockLogServer()
        {
            if( 0 != initServer()) std::cerr << "Unable To initialize Log Server\n";
        }
        LSockLogServer(const char *path): SockDomain(path)
        {
            if( 0 != initServer()) std::cerr << "Unable To initialize Log Server\n";
        }
        
        FILIKA_RESULT recv(std::string &item)
        {
            static __thread  char buff[1024];
            
            FILIKA_RESULT res = FILIKA_RESULT::FR_ERROR;
            int len = recvFrom(buff, 1024);
            if (len > 0 && len < 1024)
            {
                res = FILIKA_RESULT::FR_SUCCESS;
                item.assign(buff, len);
            }
            return res;
        }

};

class FILELOG_DECLSPEC LSockLog : public LogStream,
                                  public SockDomain
{
    public :          
        LSockLog()
        {
            init();
        }        
        LSockLog(const char *path) : SockDomain(path)
        {
            init();
        }
        ~LSockLog()
        {
            send();
        }
        FILIKA_RESULT send()
        {
            std::string s = _os.str();
            size_t  len   = s.length();
            ssize_t res   = 0;
            if (len > 0)
            {
                /*ssize_t res   = */sendTo(s.c_str(), len);
                //sendTo("", 0);
                //sendTo("xxx\n", 4);
                //sendTo("", 0);
                
            }
            
            Reset();

            return ( (res == -1) || (res != (ssize_t)len)) ? FILIKA_RESULT::FR_ERROR : FILIKA_RESULT::FR_SUCCESS;
        }
};

#define __FILEN__    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_LINE(lgr) lgr.Get(__LINE__, __func__)

/*
#define LSOCK_LOG(level) \
    if (level > FILELOG_MAX_LEVEL && level > Filika::LogStream<Filika::SockDomain>::LogLevel() )  ;  \
    else Filika::LogStream<Filika::SockDomain>().Get(level)
*/




#include <sys/time.h>

inline std::string NowTime()
{
    char buffer[11];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000); 
    return result;
}


#endif //__LOG_H__




    
    
    
    













    /*
    class LoggerServer : public Logger
    {
    public:
        LoggerServer(const char *path) : Logger(path) { if( 0 != _sock.initServer()) std::cerr << "Unable To initialize Log Server\n"; }
        LoggerServer()                                { if( 0 != _sock.initServer()) std::cerr << "Unable To initialize Log Server\n"; } 
    private :
    };
    */

    
    /*
#define __FILEN__ std::string(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_LINE(lg)          (lg).send( static_cast<std::stringstream &>(std::stringstream()<< std::to_string(__LINE__) << std::string(":") << __FILEN__ << std::string(" -> ") << __func__ ).str());
#define LOG_LINE_STR(lg, msg) (lg).send( static_cast<std::stringstream &>(std::stringstream()<< std::to_string(__LINE__) << std::string(":") << __FILEN__ << std::string(" -> ") << __func__ << " - " << std::string(msg)).str());
    //#define LOG_LINE (cfg, stm, dsc, fmt, ...) if(cfg) fprintf(stm, "%03d: %-28s: %-20s : %-35s : "  fmt "\n",   __LINE__, __FILEN__, __FUNCTION__, dsc, ##__VA_ARGS__)
//#define LOG_LINE_STR2 (lg, dsc, fmt, ...) { char bf[1024] = ""; vsnprintf(bf, 1024, "%03d: %-28s: %-20s : %-35s : "  fmt "\n",   __LINE__, __FILEN__, __FUNCTION__, dsc, ##__VA_ARGS__); (lg).send(bf); }

//#define LOG_LINE_STR2 (lg, dsc, fmt, ...) LogLine(lg, dsc, fmt, __LINE__, __FILEN__.c_str(), __FUNCTION__, __VA_ARGS__)
void LogLine(Logger &lg, const char *desc, const char *fmt, int lineNo, const char *fname, const char *funcName, ...);
void LogLine(Logger &lg, const char *desc, const char *fmt, int lineNo, const char *fname, const char *funcName, std::va_list list);
void LogLine(Logger &lg, const char *desc, const char *fmt, const char *funcName, ...);
void LogLine(Logger &lg, const char *desc, const char *fmt, const char *funcName, std::va_list list);
void LogLine(Logger &lg, const char *fmt, ...);
void LogLine(Logger &lg, const char *fmt, std::va_list list);
    */
    
} //::Filika 







#endif // __LOGGER_H__ 

