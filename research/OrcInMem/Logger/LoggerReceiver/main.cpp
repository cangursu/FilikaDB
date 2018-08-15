
#include "Logger.h"
#include "SocketDomain.h"

#include "Socket.h"
#include "SocketServer.h"
#include "SocketDomain.h"
#include "SourceChannel.h"
#include "SocketUtils.h"

#include <iostream>


#include <iostream>
#include <unistd.h>
#include <deque>
#include <functional>
#include <cstring>


#define SOCK_PATH_DEFAULT "/home/postgres/.sock_domain_log"


template <typename TSockSrv, typename TSockCln>
class ReflectServer : public SocketServer<TSockSrv, TSockCln>
{
    public:
        ReflectServer(const char *name):SocketServer<TSockSrv, TSockCln>(name)
        {
        }


        virtual void OnAccept(const TSockCln &sock, const sockaddr &addr)
        {
            /*
            std::string host, serv;
            if (true == NameInfo(addr, host, serv))
            {
                std::cout << "Accepted connection on host = " << host  << " port = " << serv << std::endl;
                ClientCount();
            }
            */
        }

        virtual void OnRecv(TSockCln &sock, MemStream<std::uint8_t> &&stream)
        {
            StreamPacket packet;

            msize_t             offsetStream = 0L;

            const msize_t        buffLen = 128;
            StreamPacket::byte_t buff [buffLen];

            SocketResult res = SocketResult::SR_ERROR_AGAIN;
            while(SocketResult::SR_ERROR_AGAIN == res)
            {
                auto reader = [&stream, &offsetStream] (char *buff, int len) -> int { return stream.read(buff, len, offsetStream); };
                res = sock.recvPacket(packet, reader);

                if (res == SocketResult::SR_ERROR_AGAIN || res == SocketResult::SR_SUCCESS)
                {
                    offsetStream += packet.BufferLen();;
                }
            }
        }
        
        void Display(StreamPacket &packet)
        {
            msize_t pyLen = packet.PayloadLen();
            for (msize_t i = 0; i < pyLen; i += buffLen)
            {
                if (packet.PayloadPart(buff, buffLen, i) > 0)
                {
                    std::cout << std::string((char*)buff, pyLen) << std::endl;
                }
            }
        }

        virtual void OnDisconnect  (const TSockCln &sock)
        {
            //std::cout << "Client Disconnected. \n";
            ClientCount();
        }

        virtual void OnErrorClient(SocketResult res)
        {
            //std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }

        virtual void OnErrorServer(SocketResult res)
        {
            //std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };

        void ClientCount()
        {
            //std::cout << "Client count = " << SocketServer<TSockSrv, TSockCln>::ClientCount() << std::endl;
        }
};


int main(int argc, char** argv)
{
    const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "Filika Logger Receiver Entered : " << sfile << std::endl;

    LogLineGlbSocketName(nullptr);

    ReflectServer<SocketDomain, SocketClientPacket<SocketDomain>> srv("ReflectServer");
    srv.SocketPath(sfile);

    if (SocketResult::SR_ERROR == srv.Init())
    {
        perror("Unable to initialize Echo Server");
    }

    srv.LoopListen();

    return 0;
}



