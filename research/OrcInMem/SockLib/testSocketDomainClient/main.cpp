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
#include "SocketDomain.h"
#include "SocketClient.h"
#include "SocketUtils.h"

#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>

#include <thread>
#include <condition_variable>



template <typename TSock>
class EchoClient : public SocketClient<TSock>
{
    public:
        EchoClient(const char *name) : SocketClient<TSock>(name) {}

        virtual void OnRecv(const TSock &, MemStream<std::uint8_t> &&)
        {
            std::cout << "OnRecv : " << std::endl;
        }
        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ERROR  : " << SocketResultText(res) << " - (" << errno <<  ") " << strerror(errno) << std::endl;
        }
};


int main()
{
    std::cout << "Domain Socket Client Test Suit V0.0\n";
 
    const void *data  = "Test";
    const char *sname = "/home/postgres/.sock_rawtest";

    EchoClient<SocketDomain> sock1("EchoClient1");
    EchoClient<SocketDomain> sock2("EchoClient2");
    sock1.SocketPath(sname);
    sock2.SocketPath(sname);

    if (SocketResult::SR_SUCCESS != sock2.Init())
    {
        std::cerr << "ERROR : Unable to init SocketClient - (" << errno <<  ") " << strerror(errno) << std::endl;
        return -1;
    }
    if (SocketResult::SR_SUCCESS != sock1.Init())
    {
        std::cerr << "ERROR : Unable to init SocketClient - (" << errno <<  ") " << strerror(errno) << std::endl;
        return -1;
    }


    if (SocketResult::SR_SUCCESS != sock2.Connect())
    {
        std::cerr << "ERROR : Unable to Connect SocketClient - (" << errno <<  ") " << strerror(errno) << std::endl;
        return -2;
    }
    if (SocketResult::SR_SUCCESS != sock1.Connect())
    {
        std::cerr << "ERROR : Unable to Connect SocketClient - (" << errno <<  ") " << strerror(errno) << std::endl;
        return -2;
    }


    if (SocketResult::SR_SUCCESS != sock2.Send(data, strlen((char*)data)))
    {
        std::cerr << "ERROR : Unable to Send data\n";
        return -3;
    }
    if (SocketResult::SR_SUCCESS != sock1.Send(data, strlen((char*)data)))
    {
        std::cerr << "ERROR : Unable to Send data\n";
        return -3;
    }

    std::cout << "\n";
    return 0;
}
