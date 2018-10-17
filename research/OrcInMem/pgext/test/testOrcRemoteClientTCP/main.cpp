
#include "SocketClientPacket.h"
#include "SocketTCP.h"
#include "SocketServer.h"
#include "Console.h"
#include "MemStreamPacketStore.h"
#include "MemStreamPacket.h"


#include <stdio.h>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

//#define SOCK_PATH_DEFAULT "/home/postgres/.sock_domain_pgext"
#define SOCK_LOG_DEFAULT  "/home/postgres/.sock_domain_log"



class MyClient : public SocketClientPacket<SocketTCP>
{
    public:
        MyClient(const char *name) : SocketClientPacket<SocketTCP>(name)
        {
        }
        virtual void OnErrorClient (SocketResult err)
        {
            std::cerr << "SocketClientPacket::OnErrorClient ->" << SocketResultText(err) << std::endl;
        }
        virtual void OnRecvPacket(StreamPacket &&packet)
        {
            msize_t         pyLen     = packet.PayloadLen();
            msize_t         pyLenRead = 0;
            const msize_t   buffLen   = 32;
            byte_t          buff [buffLen];

            std::stringstream ss;

            for (msize_t i = 0; i < pyLen; i += buffLen)
            {
                if ((pyLenRead = packet.PayloadPart(buff, buffLen, i)) > 0)
                    ss << std::string((char*)buff, pyLenRead);
            }
            std::cout << "ClientEcho  - PGEXT Packet: \n" << ss.str() << std::endl;
        }


        SocketResult SendPacketJSN(const char *content)
        {
            return SendPacketJSN(content, std::strlen(content));
        }

        SocketResult SendPacketJSN(const void *content, size_t length)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            packet.CreatePacketWrite("TableXXX", content, length);

            if (SocketResult::SR_SUCCESS != (res = this->SendPacket(packet)))
            {
                LOG_LINE_GLOBAL("remote", "ERROR: Unable to send packet. errno:", errno);
                std::cout << "ERROR : SendPacket failed.\n";
            }

            return res;
        }
};




// *************************************************************************
// ***                                                                   ***
// ***                   APP                                             ***
// ***                                                                   ***
// ***                                                                   ***
// *************************************************************************

int main(int argc, char** argv)
{
    LogLineGlbSocketName("/home/postgres/.sock_domain_log");

    //const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "\nRemote ORC Test Client\n";


    MyClient client ("TestClient");
    client.Address("127.0.0.1", 5001);
    if (SocketResult::SR_SUCCESS != client.Init())
    {
        std::cerr << "Init ERROR\n";
        return -1;
    }
    if (SocketResult::SR_SUCCESS != client.Connect())
    {
        std::cerr << "Connect ERROR\n";
        return -2;
    }


    client.SendPacketJSN("");
    /*
    client.SendPacketJSN("1234567890");
    client.SendPacketJSN("ABCDEFGHIYKLMNOPQRSTUVWXYZ");
    client.SendPacketJSN("ABCDEFGHIYKLMNOPQRSTUVWXYZabcdefghiyklmnopqrstuvwxyz");
    client.SendPacketJSN("1234567890ABCDEFGHIYKLMNOPQRSTUVWXYZabcdefghiyklmnopqrstuvwxyz0987654321");
    client.SendPacketJSN("ORC");
    */


    msleep(100);
    std::cout << "\nRemote ORC Test Client finished\n";
    return 0;
}



