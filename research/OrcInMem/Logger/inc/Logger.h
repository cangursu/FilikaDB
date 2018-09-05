/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Logger.h
 * Author: can.gursu@medyasoft.com.tr
 *
 * Created on April 19, 2018, 9:44 AM
 */


#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "SocketDomain.h"

#include <iostream>
#include <sstream>


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















namespace  Filika
{


#ifndef __LOG_H__
#define __LOG_H__



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
    _os.str(std::move(std::string()));
    _os .clear();

    _os << NowTime() << " " << ToString(level) << " - ";
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



class FILELOG_DECLSPEC LSockLog : public LogStream,
                                  public SocketDomain
{
    public :
        LSockLog(const char *path = SOCK_PATH_DEFAULT, bool doConnect = true);
        ~LSockLog();

        SocketResult SendRaw();
        SocketResult SendPacket();

private :

        SocketResult SendRaw(const void *p, int l);
};


#define __FILEN__    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_LINE(lgr) lgr.Get(__LINE__, __func__)

/*
#define LSOCK_LOG(level) \
    if (level > FILELOG_MAX_LEVEL && level > Filika::LogStream<Filika::SocketDomain>::LogLevel() )  ;  \
    else Filika::LogStream<Filika::SocketDomain>().Get(level)
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

} //::Filika
#endif // __LOGGER_H__
