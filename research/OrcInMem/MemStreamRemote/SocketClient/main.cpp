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
#include "SocketUtils.h"

#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>

#include <thread>
#include <condition_variable>




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
        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ERROR : " << SocketResultText(res) << std::endl;
        }
};



std::mutex m;
std::condition_variable cv;
bool ready = false;


void thread_function(EchoClient *sock, const std::string &prefix , int id)
{
    std::cout << "thread_function : " << id << std::endl;
    //EchoClient sock;

    std::cout << "thread_function : waiting " << id << std::endl;
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{return ready;});

    std::cout << "thread_function : sending " << id << std::endl;

    std::string str = prefix + " - " + std::to_string(id);
    sock->Send((void *)str.c_str(), str.size());

    std::cout << "thread_function : " << id << " done \n";
    //sleep(1);
}

int main()
{
    const int count = 64;//2 * 1024;
    std::vector<std::thread> ths;

    EchoClient sock[2*count];

    std::string p1 = "abcd";
    for (int i = 0; i < count; ++i)
    {
        ths.push_back( std::thread(thread_function, &sock[i], p1, i) );
    }

    std::string p2 = "wxyz";
    for (int i = 0; i < count; ++i)
    {
        ths.push_back( std::thread(thread_function, &sock[i+count], p2, i+count) );
    }

    std::cout << "Sleepimg\n";
    sleep(5);
    std::cout << "Fireing\n";


    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_all();


    for(auto &th : ths) th.join();

    std::cout << "Done\n";
    sleep(2);
    std::cout << "Quiting\n";


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

