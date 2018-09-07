

#include "SocketServer.h"
#include "SocketClient.h"
#include "SocketDomain.h"
#include "SocketUtils.h"

#include <gtest/gtest.h>
#include <thread>
#include <string>



int nsleep(long miliseconds)
{
   struct timespec req, rem;

   if(miliseconds > 999)
   {
        req.tv_sec  = (int)(miliseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (miliseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }
   else
   {
        req.tv_sec  = 0;                         /* Must be Non-Negative */
        req.tv_nsec = miliseconds * 1000000;    /* Must be in range of 0 to 999999999 */
   }

   return nanosleep(&req , &rem);
}

std::string MemStream2String(MemStream<std::uint8_t> &&stream)
{
    std::string     strData;
    std::uint64_t   rsize = 0;
    const int       bsize = 256;
    char            buff[bsize];

    for (std::uint64_t offset = 0, len = stream.Len(); offset < len; offset += bsize)
    {
        memset(buff, 0, bsize);
        rsize = stream.read(buff, bsize, offset);
        strData += std::string(buff, rsize);
    }

    return std::move(strData);
}


template <typename TSocket>
class TestServerClient : public SocketClient<TSocket>
{
    public:
        TestServerClient(const char *name = "TestServerClient")
            : SocketClient<TSocket>(name)
        {
        }
        TestServerClient(int fd, const char *name)
            : SocketClient<TSocket>(fd, name)
        {
        }

        virtual void OnRecv (MemStream<std::uint8_t> &&stream)
        {
            if (SocketResult::SR_SUCCESS == this->Send(stream))
            {
                _testContent  = "Step1-";
                _testContent += MemStream2String(std::move(stream));
            }
            else
            {
                std::cout << "ERRORRRR......\n";
            }
            //std::cout << "TestServerClient::OnRecv -> " << stream.Len() << ":" << _testContent << std::endl;
        }


        virtual void OnErrorClient (SocketResult res)
        {
            std::cout << "ErrorClient::OnErrorClient - " << SocketResultText(res) << std::endl;
        }

        std::string  _testContent = "Empty_TestServerClient";
};


template <typename TSockSrv, typename TSockCln>
class TestServer : public SocketServer<TSockSrv, TSockCln>
{
    public:
        TestServer(const char *name = "TestServer"):SocketServer<TSockSrv, TSockCln>(name)
        {
        }
        virtual void OnAccept(const TSockCln &sock, const sockaddr &addr)
        {
            //std::cout << "Connection Accepted  \n";
            //ClientCountMsg();
        }
        virtual void OnDisconnect  (const TSockCln &sock)
        {
            //std::cout << "Client Disconnected. \n";
            //ClientCountMsg();
        }
        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }
        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };
        void ClientCountMsg()
        {
            std::cout << "Client count = " << SocketServer<TSockSrv, TSockCln>::ClientCount() << std::endl;
        }

        std::string TestData(int idx)
        {
            const TestServerClient<SocketDomain> *pp = this->ClientByIdx(idx);
            //auto p = this->ClientByIdx(1);
            if (pp)
                return pp->_testContent;
            return "";
        }

};




template <typename TSock>
class TestClient : public SocketClient<TSock>
{
    public:
        TestClient(const char *name = "TestClient") : SocketClient<TSock>(name) {}

        virtual void OnRecv(MemStream<std::uint8_t> &&stream)
        {
            _testContent  = "Step2-";
            _testContent += MemStream2String(std::move(stream));

            //std::cout << "TestClient::OnRecv -> " << stream.Len() << ":" << _testContent << std::endl;
        }
        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ERROR  : " << SocketResultText(res) << " - (" << errno <<  ") " << strerror(errno) << std::endl;
        }
        std::string  _testContent = "Empty_TestClient";
};



TEST(SocketDomain, Transmission_Echo)
{
    TestServer<SocketDomain, TestServerClient<SocketDomain>> srv("TestServerGTest");
    TestClient<SocketDomain>                                 cln("TestClient");

    srv.SocketPath(".sock_rawtest");
    cln.SocketPath(".sock_rawtest");

    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    ASSERT_EQ (SocketResult::SR_SUCCESS, cln.Init());

    SocketResult resSrv, resCln;
    std::thread thSrv ( [&srv, &resSrv]() {resSrv = srv.LoopListen();} );
    std::thread thCln ( [&cln, &resCln]() {resCln = cln.LoopRead();  } );

    ASSERT_EQ (SocketResult::SR_SUCCESS, cln.Connect());
    nsleep(1);
    ASSERT_EQ (1, srv.ClientCount());

    const char *p = "TestContent";
    ASSERT_EQ (SocketResult::SR_SUCCESS, cln.Send(p, std::strlen(p)));
    nsleep(1);

    ASSERT_STREQ ((std::string("Step1-") + p).c_str(), srv.TestData(0).c_str());
    ASSERT_STREQ ((std::string("Step2-") + p).c_str(), cln._testContent.c_str());

    srv.LoopListenStop();
    cln.LoopReadStop();

    thSrv.join();
    thCln.join();

    ASSERT_EQ (SocketResult::SR_SUCCESS, resSrv);
    ASSERT_EQ (SocketResult::SR_SUCCESS, resCln);
}

TEST(SocketDomain, LoopListen1)
{
    TestServer<SocketDomain, TestServerClient<SocketDomain>> srv("TestServer");
    srv.SocketPath(".sock_rawtest");

    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.LoopListenPrepare());

    for(int i = 0; i < 5; ++i)
    {
        ASSERT_EQ (SocketResult::SR_SUCCESS, srv.LoopListenSingleShot());
    }

    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Release());
}


TEST(SocketDomain, LoopListen2)
{
    TestServer<SocketDomain, TestServerClient<SocketDomain>> srv("TestServerGTest");
    srv.SocketPath(".sock_rawtest");

    SocketResult res = srv.Init();
    ASSERT_EQ (SocketResult::SR_SUCCESS, res);

    std::thread th ([&srv, &res]() {res = srv.LoopListen();});
    nsleep(50);
    srv.LoopListenStop();
    ASSERT_EQ (SocketResult::SR_SUCCESS, res);
    th.join();
}

TEST(SocketDomain, InitServer)
{
    TestServer<SocketDomain, TestServerClient<SocketDomain>> srv("TestServerGTest");
    srv.SocketPath(".sock_rawtest");
    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Init());
}

TEST(SocketDomain, InitClient)
{
    TestClient<SocketDomain> sock("TestClient");
    ASSERT_EQ (SocketResult::SR_SUCCESS, sock.Init());
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

