
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
            DisplayPacket("Recieved Packet", packet);

            std::vector <MemStreamPacket::Cmd> cmds;
            MemStreamPacket stream;
            int count = stream.DecodePacket(packet, cmds);
            for (const auto &cmd : cmds)
            {
                std::cout << cmd.Dump() << std::endl;
            }

        }

        SocketResult SendPacketWrite(const char *source, const char *content)
        {
            return SendPacketWrite(source, content, std::strlen(content));
        }

        SocketResult SendPacketWrite(const char *source, const void *content, size_t length)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            packet.CreatePacketWrite("TEST-MyClient", source, length, content);

            if (SocketResult::SR_SUCCESS != (res = this->SendPacket(packet)))
            {
                LOG_LINE_GLOBAL("remote", "ERROR: Unable to send packet. errno:", errno);
                std::cout << "ERROR : SendPacket failed.\n";
            }
            else
            {
                //DisplayPacket("Sended Packet", packet);
            }

            return res;
        }

        SocketResult SendPacketRead(const char *source, size_t length, size_t offset)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            packet.CreatePacketRead("TEST-MyClient", source, length, offset);

            if (SocketResult::SR_SUCCESS != (res = this->SendPacket(packet)))
            {
                LOG_LINE_GLOBAL("remote", "ERROR: Unable to send packet. errno:", errno);
                std::cout << "ERROR : SendPacket failed.\n";
            }
            else
            {
                //DisplayPacket("Sended Packet", packet);
            }

            return res;
        }


        SocketResult SendPacketDelete(const char *source)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            packet.CreatePacketDelete("TEST-MyClient", source);

            if (SocketResult::SR_SUCCESS != (res = this->SendPacket(packet)))
            {
                LOG_LINE_GLOBAL("remote", "ERROR: Unable to send packet. errno:", errno);
                std::cout << "ERROR : SendPacket failed.\n";
            }
            else
            {
                //DisplayPacket("Sended Packet", packet);
            }

            return res;
        }


        SocketResult SendPacketLength(const char *source)
        {
            SocketResult res = SocketResult::SR_EMPTY;
            MemStreamPacket packet;
            packet.CreatePacketLength("TEST-MyClient", source);

            if (SocketResult::SR_SUCCESS != (res = this->SendPacket(packet)))
            {
                LOG_LINE_GLOBAL("remote", "ERROR: Unable to send packet. errno:", errno);
                std::cout << "ERROR : SendPacket failed.\n";
            }
            else
            {
                //DisplayPacket("Sended Packet", packet);
            }

            return res;
        }

        void DisplayPacket(const char *msg, StreamPacket &packet)
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

            std::cout << "ORC Remote Client  - " << msg << " : \n" << ss.str() << std::endl;
        }

};



// *************************************************************************
// ***    Console Implementation                                         ***
// *************************************************************************

/*
#define KEY_EVENT_ID_HELP           0
#define KEY_EVENT_ID_QUIT           1
#define KEY_EVENT_ID_LF             2


template <typename TServer>
class RemoteClientConsole : public Console
{
public:
    TServer &_srv;
    RemoteClientConsole(TServer &srv) : _srv(srv)
    {
    }

    virtual void EventFired(const Console::KeyHandler &kh)
    {
        switch(kh._id)
        {
            case KEY_EVENT_ID_HELP          :   DisplayHelp();  break;
            case KEY_EVENT_ID_QUIT          :   _srv.LoopListenStop(); LoopStop();  break;
            case KEY_EVENT_ID_LF            :   for(int i = 0; i < 24; i++) std::cout << std::endl; break;
        }
    }
};

*/




// *************************************************************************
// ***                                                                   ***
// ***                   APP                                             ***
// ***                                                                   ***
// ***                                                                   ***
// *************************************************************************

/*
int main(int argc, char** argv)
{
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

    std::thread th ([&client]()->void{client.LoopStart();});
    client.SendPacketWrite("TableXXX", "1234567890");
    if (th.joinable()) th.join();
}
*/

int main(int argc, char** argv)
{
    LogLineGlbSocketName("/home/postgres/.sock_domain_log");

    //const char *sfile = (argc > 1) ? argv[1] : SOCK_PATH_DEFAULT;
    std::cout << "\nRemote ORC Test Client\n";

    MyClient client ("TestClient");
    client.Address("127.0.0.1", 5001);


    //RemoteClientConsole<MyClient> con(client);

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

    std::thread th ([&client]()->void{client.LoopStart();});

/*
    client.SendPacketWrite("TableXXX", "");
    client.SendPacketWrite("TableXXX", "1234567890");
*/

    //client.SendPacketWrite("TableXXX", "ABCDEFGHIYKLMNOPQRSTUVWXYZ");
    client.SendPacketLength("TableXXX");
/*
    client.SendPacketWrite("TableYYY", "ABCDEFGHIYKLMNOPQRSTUVWXYZ");
    client.SendPacketWrite("TableXXX", "ABCDEFGHIYKLMNOPQRSTUVWXYZabcdefghiyklmnopqrstuvwxyz");
    client.SendPacketWrite("TableXXX", "1234567890ABCDEFGHIYKLMNOPQRSTUVWXYZabcdefghiyklmnopqrstuvwxyz09876543211234567890ABCDEFGHIYKLMNOPQRSTUVWXYZabcdefghiyklmnopqrstuvwxyz0987654321");
    client.SendPacketWrite("TableXXX",
                            "\nEy Türk Gençliği!\n\n"
                            "Birinci vazifen, Türk istiklâlini, Türk Cumhuriyetini, ilelebet, muhafaza ve müdafaa etmektir.\n\n"
                            "Mevcudiyetinin ve istikbalinin yegâne temeli budur. Bu temel, senin, en kıymetli hazinendir. İstikbalde dahi, seni bu hazineden mahrum etmek isteyecek, dahilî ve haricî bedhahların olacaktır. Bir gün, İstiklâl ve Cumhuriyeti müdafaa mecburiyetine düşersen, vazifeye atılmak için, içinde bulunacağın vaziyetin imkân ve şerâitini düşünmeyeceksin! Bu imkân ve şerâit, çok nâmüsait bir mahiyette tezahür edebilir. İstiklâl ve Cumhuriyetine kastedecek düşmanlar, bütün dünyada emsali görülmemiş bir galibiyetin mümessili olabilirler. Cebren ve hile ile aziz vatanın, bütün kaleleri zaptedilmiş, bütün tersanelerine girilmiş, bütün orduları dağıtılmış ve memleketin her köşesi bilfiil işgal edilmiş olabilir. Bütün bu şerâitten daha elîm ve daha vahim olmak üzere, memleketin dahilinde, iktidara sahip olanlar gaflet ve dalâlet ve hattâ hıyanet içinde bulunabilirler. Hattâ bu iktidar sahipleri şahsî menfaatlerini, müstevlilerin siyasi emelleriyle tevhit edebilirler. Millet, fakr ü zaruret içinde harap ve bîtap düşmüş olabilir.\n\n"
                            "Ey Türk istikbalinin evlâdı! İşte, bu ahval ve şerâit içinde dahi, vazifen; Türk İstiklâl ve Cumhuriyetini kurtarmaktır! Muhtaç olduğun kudret, damarlarındaki asil kanda mevcuttur!\n\n"
                            "Mustafa Kemal Atatürk\n\n"
                            "20 Ekim 1927"   );
    client.SendPacketRead("TableXXX", 10, 30);
    client.SendPacketRead("TableYYY", 10, 5);
    client.SendPacketDelete("TableXXX");
    client.SendPacketDelete("TableYYY");
    client.SendPacketDelete("TableTTT");
*/
    msleep(200);
    client.LoopStop();
    if (th.joinable()) th.join();

    std::cout << "\nRemote ORC Test Client finished\n";
    return 0;
}

