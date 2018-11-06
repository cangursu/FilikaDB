
/*
 * File:   main.cpp
 * Author: can.gursu@medyasoft.com.tr
 *
 * Created on April 19, 2018, 12:08 PM
 */


#include "Logger.h"
#include "LoggerGlobal.h"

#include <unistd.h>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <vector>


/*
 *
 */
int main(int argc, char** argv)
{
    const char *sFile = (argc > 1) ? argv[1] : "/home/postgres/.sock_domain_log" /*SOCK_PATH_DEFAULT*/;
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
    LOG_LINE_GLOBAL("deneme", "columnar_store_load");
*/

   /*
    Filika::LogStream<Filika::SocketDomain> lg(sFile);
    lg.get() << "Kello\n";
   */


    //LSOCK_LOG(Filika::TLogStreamLevel::LSL_INFO) << "Mello";

    /*
    {
        Filika::LSockLog lg(sFile);
        lg.Get() << "Deneme .... " << std::endl;
        lg.Get() << "Keneme .... " << std::endl;
        lg.Get() << "Meneme .... " << std::endl;
        lg.Get() << "Meleme .... " << std::endl;
        lg.send();

        LOG_LINE(lg) << "Bu da başka deneme";
    }
    */

    LogLineGlbSocketName(sFile);

    std::string desc (std::move(std::string("TestLog:") + std::to_string(getpid())));
    const char *pdesc = desc.c_str();

    {
        LOG_LINE_GLOBAL(pdesc, "Test- -> ", 1111, " <- olayi");
        LOG_LINE_GLOBAL(pdesc, "Test- -> ", 2222, " <- olayi");
    }
    {
        LOG_LINE_GLOBAL(pdesc, "Test- -> ", 3333, " <- olayi");
    }
    {
        LOG_LINE_GLOBAL(pdesc, "Test- -> ", 4444, " <- olayi");
    }
    {
        LOG_LINE_GLOBAL(pdesc, "Test- -> ", 5555, " <- olayi");
    }


    for (int i = 0; i < 10000; ++i)
    {
        LOG_LINE_GLOBAL(pdesc, "Global Log = ", std::to_string(i), " . ");
    }



//    sleep(15);




/*
    std::vector<std::thread> thList;
    for (int thNo = 0; thNo < 2; thNo++)
    {
        thList.push_back(std::thread(
                             [thNo]()
                             {
                                 for (int idx = 0; idx < 1; idx++)
                                     LOG_LINE_GLOBAL(pdesc, "thNo = ", std::to_string(thNo), ", idx = ", idx);
                             }
                            )
                        );
    }
    for (auto &t : thList) t.join();
*/




    std::cout << "Filika Logger Sender: Quited\n";
    return 0;
}

