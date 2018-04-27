
/* 
 * File:   main.cpp
 * Author: postgres
 *
 * Created on April 19, 2018, 12:08 PM
 */

#include <cstdlib>
#include <iostream>

#include "Logger.h"

/*
 * 
 */
int main(int argc, char** argv)
{
    std::cout << "Filika Logger Sender\n";
    Filika::Logger  logObject("/home/postgres/logsocket");
    
    logObject.send("Log Item");
    LOG_LINE(logObject);
    
    int sum = 0;
    //for (int i = 0; i < 10000; ++i)
    //    for (int i = 0; i < 10000; ++i) LOG_LINE_STR(logObject, std::to_string(++sum));
  
        
    LogLine(logObject, "Test.1", "%d", __LINE__, __FILEN__.c_str(), __FUNCTION__, i);
    LogLine(logObject, "Test.2", "%d", __FUNCTION__, i);
    LogLine(logObject, "Test.3 : %d", i);
    LogLine(logObject, "root->type : %s", "test");

    return 0;
}

