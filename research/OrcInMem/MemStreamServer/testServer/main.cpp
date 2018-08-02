/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: can.gursu
 *
 * Created on 16 July 2018
 */


#include "SourceChannel.h"
#include "LoggerGlobal.h"


#include <iostream>
#include <thread>


int main(int argc, char** argv)
{
    std::cout << "StreamServerClient v0\n";

    const char *slog = "/home/postgres/.sock_memserverlog";
    std::cout << "Using LogServer : " << slog << std::endl;
    LogLineGlbSocketName(slog);
    LOG_LINE_GLOBAL("SServerClient", "ver 0.0.0.0");


    /*std::thread th = */SourceChannel::listenAsyc("/home/postgres/.pgext_domain_sock");
    //th.join();

    return 0;
}

