/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "main.h"

#include "SocketDomain.h"
#include "SocketUtils.h"

#include <gtest/gtest.h>

#include <thread>
#include <string>





TEST(SocketDomain, InitClient)
{
    TestClient<SocketDomain> sock("TestClient");
    ASSERT_EQ (SocketResult::SR_SUCCESS, sock.Init());
}


TEST(SocketDomain, InitServer)
{
    TestServer<SocketDomain, TestServerClient<SocketDomain>> srv("TestServerGTest");
    srv.SocketPath(".sock_rawtest");
    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Init());
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
    msleep(50);
    srv.LoopListenStop();
    EXPECT_EQ (SocketResult::SR_SUCCESS, res);
    th.join();
}

TEST(SocketDomain, Connect)
{
    SocketResult resSrv;
    TestServer<SocketDomain, TestServerClient<SocketDomain>> srv("TestServerGTest");
    srv.SocketPath(".sock_rawtest");
    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    std::thread thSrv ( [&srv, &resSrv]() {resSrv = srv.LoopListen();} );

    TestClient<SocketDomain> cln("TestClient");
    cln.SocketPath(".sock_rawtest");
    ASSERT_EQ (SocketResult::SR_SUCCESS, cln.Init());
    ASSERT_EQ (SocketResult::SR_SUCCESS, cln.Connect());

    msleep(2);
    EXPECT_EQ (1, srv.ClientCount());


    srv.LoopListenStop();
    if (thSrv.joinable()) thSrv.join();

    ASSERT_EQ (SocketResult::SR_SUCCESS, resSrv);
}

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

    ASSERT_EQ (SocketResult::SR_SUCCESS, cln.Connect());
    std::thread thCln ( [&cln, &resCln]() {resCln = cln.LoopRead();  } );

    msleep(1);
    EXPECT_EQ (1, srv.ClientCount());

    std::string data = "TestContent";
    EXPECT_EQ (SocketResult::SR_SUCCESS, cln.Send(data.c_str(), data.size()));

    msleep(1);
    EXPECT_STREQ (data.c_str(), srv.TestData(0).c_str());
    EXPECT_STREQ ((data + "::From-TestServerClient").c_str(), cln._testContent.c_str());

    srv.LoopListenStop();
    cln.LoopReadStop();

    thSrv.join();
    thCln.join();

    ASSERT_EQ (SocketResult::SR_SUCCESS, resSrv);
    ASSERT_EQ (SocketResult::SR_SUCCESS, resCln);
}

