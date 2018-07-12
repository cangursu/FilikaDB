/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: can.gursu
 *
 * Created on 10 July 2018, 10:27
*/

#include "Socket.h"



#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <cstdio>
#include <iostream>





class EchoClient : public SocketClient
{
    public:
        EchoClient(const char *ip = "127.0.0.1", uint port = 5000)
        {
            SocketClient::Init();
            SocketClient::Connect(ip, port);
        }
        virtual void OnRecv(const Socket &, MemStream<std::uint8_t> &&)
        {
        }
        virtual void OnErrorClient(SocketResult)
        {
        }
};



int main()
{
    EchoClient sock [1024];
    for (int i = 0; i < 1024; ++i)
    {
        sock[i].Send((void *)("12345"), 5);
    }



    /*
    EchoClient sock;
    sock.Init();
    sock.Connect("127.0.0.1", 5000);

    char send_data[1024];
    do
    {
        std::cout << "\nSEND (q or Q to quit) : ";
        std::cin >> send_data;

        MemStream<std::uint8_t> stream;
        stream.write(send_data, strlen(send_data));


        if (SocketResult::SUCCESS != sock.Send(std::move(stream)))
            break;

    } while((strcmp(send_data , "q") != 0 && strcmp(send_data , "Q") != 0));

    return 0;
    */
}

