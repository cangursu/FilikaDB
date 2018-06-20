/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LoggerGlobal.h
 * Author: postgres
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


// #ifdef LOG_GLOBAL_ENABLE
    EXTERN_C void LogLineGlbSocketName(const char *path);
    EXTERN_C void LogLineGlobalFormat(const char *desc, int lineNo, const char *fname, const char *funcName, const char *fmt, ...) __attribute__ ((format (gnu_printf, 5, 6)));
    
    #define LOG_LINE_GLOBAL(desc, fmt, ...) LogLineGlobalFormat(desc, __LINE__, __FILENC__, __func__, fmt, ##__VA_ARGS__)
/*
    EXTERN_C void LogLineGlb_3(const char *desc, const char *fmt, int lineNo, const char *fname, const char *funcName, ...);
    EXTERN_C void LogLineGlb_2(const char *desc, const char *fmt, const char *funcName, ...);
    EXTERN_C void LogLineGlb_1(const char *fmt, ...);
// #endif  //LOG_GLOBAL_ENABLE

    #define LOG_LINE_GLOBAL(desc, fmt, ...) LogLineGlb_3(desc, fmt, __LINE__, __FILENC__, __func__, ##__VA_ARGS__)
    
    //EXTERN_C void LogLineGlobal(const char *fmt, ...);
    //    #define LOG_LINE_GLOBAL2 (stm, dsc, fmt, ...) fprintf(stm, "%03d: %-28s: %-20s : %-35s : "  fmt "\n",   __LINE__, __FILEN__, __FUNCTION__, dsc, __VA_ARGS__)
*/

#endif // __LOGGER_GLOBAL_H__ 

