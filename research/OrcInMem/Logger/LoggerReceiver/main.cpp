
#include "SocketClientPacket.h"
#include "SocketServer.h"


#define SOCK_PATH_DEFAULT "/home/postgres/.sock_domain_log"


class ReflectClient : public SocketClientPacket<SocketDomain>
{
public:
    ReflectClient(const char *name = "ReflectClient") : SocketClientPacket(name)
    {
    }

    ReflectClient(int fd, const char *name) : SocketClientPacket(fd, name)
    {
    }

    virtual void OnRecvPacket(StreamPacket &&packet)
    {
        Display(packet);
    }

    void Display(StreamPacket &packet)
    {
        const msize_t        buffLen = 512;
        StreamPacket::byte_t buff [buffLen];

        msize_t pyLenPart = 0;
        msize_t pyLen = packet.PayloadLen();
        for (msize_t offset = 0; offset < pyLen; offset += buffLen)
        {
            if ( (pyLenPart = packet.PayloadPart(buff, buffLen, offset)) > 0)
                std::cout << std::string((char*)buff, pyLenPart);
        }
        std::cout << std::endl;
    }
};


template <typename TSockSrv, typename TSockCln>
class ReflectServer : public SocketServer<TSockSrv, TSockCln>
{
    using  msize_t = StreamPacket::msize_t;

    public:

        ReflectServer(const char *name):SocketServer<TSockSrv, TSockCln>(name)
        {
        }


        virtual void OnAccept(const TSockCln &sock, const sockaddr &addr)
        {
            std::cout << "Accepted connection - ";
            ClientCount();
        }

        virtual void OnRecv(TSockCln &sock, MemStream<std::uint8_t> &&stream)
        {
            //std::cout << stream.dump("ReflectServer::OnRecv");
            sock.OnRecv(std::move(stream));
        }

        virtual void OnDisconnect  (const TSockCln &sock)
        {
            std::cout << "Client Disconnected - ";
            ClientCount();
        }

        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }

        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };

        void ClientCount()
        {
            std::cout << "Client count = " << SocketServer<TSockSrv, TSockCln>::ClientCount() << std::endl;
        }
};


int main(int argc, char** argv)
{
    const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "Filika Logger Receiver " << std::endl;

    LogLineGlbSocketName(nullptr);


    ReflectServer<SocketDomain, ReflectClient> srv("ReflectServer");
    srv.SocketPath(sfile);

    if (SocketResult::SR_ERROR == srv.Init())
    {
        perror("Unable to initialize Echo Server");
    }

    srv.LoopListen();

    return 0;
}



