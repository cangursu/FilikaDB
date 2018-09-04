/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: can.gursu
 *
 * Created on 13 August 2018
 */

#include "LoggerGlobal.h"

#include <iostream>
#include <unistd.h>



int main(int , char** )
{
    char *sname = "/home/postgres/.sock_domain_log";

    std::cout << "StreamServerClient v0.0.0.2\n";
    std::cout << "Using domain Socket : " << sname << std::endl;
    LogLineGlbSocketName(sname);

    std::string strTest("Test");
    strTest += std::to_string(getpid());

    LOG_LINE_GLOBAL("LogClient", strTest);

    for (int i = 0; i<200000; ++i)
        LOG_LINE_GLOBAL("LogClient", strTest + "-" + std::to_string(i));

    return 0;
}

