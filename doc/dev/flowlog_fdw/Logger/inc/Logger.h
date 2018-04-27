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

#ifndef __LOGGER_H__
#define __LOGGER_H__


#include <sys/socket.h>
#include <sys/un.h>

#include <string>
#include <sstream>


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
        SockDomain(SockDomain &) {} 
        SockDomain(SockDomain &&) {} 
        SockDomain(const char *path); 
        virtual ~SockDomain() ;

        int init();
        int init(const char *path);
        int initServer();
        int release() ;

        int recvFrom(void *pdata, size_t len) ;
        int sendTo(const void *pdata, size_t len);

    private:
        int _sock = -1;
        sockaddr_un _addr{AF_UNIX, SOCK_PATH_DEFAULT};
    };



    class Logger 
    {
        
    public:
        
        Logger();
        Logger(const char *path);
        Logger(const Logger& orig)  = delete;
        Logger(const Logger&& orig) = delete;
        virtual ~Logger();
    
        FILIKA_RESULT send(const std::string &item);
        FILIKA_RESULT recv(std::string &item);
        
    protected:
        SockDomain _sock;
    };
    
    class LoggerServer : public Logger
    {
    public:
        LoggerServer() { _sock.initServer(); } 
        LoggerServer(const char *path) : Logger(path) { _sock.initServer(); }
    private :
    };

    
    
#define __FILEN__ std::string(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_LINE(lg)          (lg).send( static_cast<std::stringstream &>(std::stringstream()<< std::to_string(__LINE__) << std::string(":") << __FILEN__ << std::string(" -> ") << __func__ ).str());
#define LOG_LINE_STR(lg, msg) (lg).send( static_cast<std::stringstream &>(std::stringstream()<< std::to_string(__LINE__) << std::string(":") << __FILEN__ << std::string(" -> ") << __func__ << " - " << std::string(msg)).str());
    //#define LOG_LINE (cfg, stm, dsc, fmt, ...) if(cfg) fprintf(stm, "%03d: %-28s: %-20s : %-35s : "  fmt "\n",   __LINE__, __FILEN__, __FUNCTION__, dsc, ##__VA_ARGS__)
//#define LOG_LINE_STR2 (lg, dsc, fmt, ...) { char bf[1024] = ""; vsnprintf(bf, 1024, "%03d: %-28s: %-20s : %-35s : "  fmt "\n",   __LINE__, __FILEN__, __FUNCTION__, dsc, ##__VA_ARGS__); (lg).send(bf); }

//#define LOG_LINE_STR2 (lg, dsc, fmt, ...) LogLine(lg, dsc, fmt, __LINE__, __FILEN__.c_str(), __FUNCTION__, __VA_ARGS__)
void LogLine(Logger &lg, const char *desc, const char *fmt, int lineNo, const char *fname, const char *funcName, ...);
void LogLine(Logger &lg, const char *desc, const char *fmt, const char *funcName, ...);
void LogLine(Logger &lg, const char *fmt, ...);


}







#endif /* __LOGGER_H__ */

