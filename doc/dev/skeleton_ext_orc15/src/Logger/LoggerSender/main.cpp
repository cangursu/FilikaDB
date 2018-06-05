
/* 
 * File:   main.cpp
 * Author: postgres
 *
 * Created on April 19, 2018, 12:08 PM
 */

#include <cstdlib>
#include <iostream>

#include "Logger.h"
#include "LoggerGlobal.h"
#include <unistd.h>
/*
 * 
 */
int main(int argc, char** argv)
{
    const char *sFile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "Filika Logger Sender Entered : " << sFile << std::endl;
/*
    {
        Filika::Logger  logObject(sFile);

        logObject.send("Log Item");
        LOG_LINE(logObject);

        int sum = 0;
        //for (int i = 0; i < 10000; ++i)
        //    for (int i = 0; i < 10000; ++i) LOG_LINE_STR(logObject, std::to_string(++sum));


        LogLine(logObject, "Test.1", "%d", __LINE__, __FILEN__.c_str(), __FUNCTION__, sum);
        LogLine(logObject, "Test.2", "%d", __FUNCTION__, sum);
        LogLine(logObject, "Test.3 : %d", sum);
        LogLine(logObject, "root->type : %s", "test");
    }

    
    LogLineGlbSocketName (sFile);
    LogLineGlb_1("LogLineGlb_1");
    LogLineGlb_2("LogLineGlb_2", "Deneme %d", __FILEN__.c_str(), 5);
    LogLineGlb_3("LogLineGlb_3", "Deneme %d", __LINE__, __FILEN__.c_str(), __func__, 5 );
*/
/*
    LogLineGlbSocketName(sFile);
    LOG_LINE_GLOBAL("deneme","%s", "columnar_store_load");
*/  
 
   /*
    Filika::LogStream<Filika::SockDomain> lg(sFile);
    lg.get() << "Kello\n";
   */       
            
    
    //LSOCK_LOG(Filika::TLogStreamLevel::LSL_INFO) << "Mello";

    {
        Filika::LSockLog lg(sFile);
        lg.Get() << "Deneme .... " << std::endl;
        lg.Get() << "Keneme .... " << std::endl;
        lg.Get() << "Meneme .... " << std::endl;
        lg.Get() << "Meleme .... " << std::endl;
        lg.send();
        
        LOG_LINE(lg) << "Bu da başka deneme";
    }          
    
 
    {
        LogLineGlbSocketName(sFile);
        LOG_LINE_GLOBAL("desc olayi", "test -> %s", "olayı");
    }
    {    
        LOG_LINE_GLOBAL("desc olayi", "Mest -> %s", "olayı");
    }
    {    
        LOG_LINE_GLOBAL("desc olayi", "Pest -> %s", "olayı");
    }
    {    
        LOG_LINE_GLOBAL("desc olayi", "Rest -> %s", "olayı");
    }
    
    std::cout << "Filika Logger Sender: Quited\n";
    return 0;
}

