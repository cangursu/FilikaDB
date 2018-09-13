/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "main.h"
#include "SocketTCP.h"

#include <gtest/gtest.h>
#include <thread>


TEST(SocketTCP, LoopListen1)
{
    TestServer<SocketTCP, TestServerClient<SocketTCP>> srv("EchoServer");
    srv.Port(5000);
    EXPECT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    EXPECT_EQ (SocketResult::SR_SUCCESS, srv.LoopListenPrepare());

    for(int i = 0; i < 5; ++i)
    {
        EXPECT_EQ (SocketResult::SR_SUCCESS, srv.LoopListenSingleShot());
    }

    EXPECT_EQ (SocketResult::SR_SUCCESS, srv.Release());
}


TEST(SocketTCP, LoopListen2)
{
    TestServer<SocketTCP, TestServerClient<SocketTCP>> srv("TestServerGTest");
    srv.Port(5000);

    SocketResult res = srv.Init();
    EXPECT_EQ (SocketResult::SR_SUCCESS, res);

    std::thread th ([&srv, &res]() {res = srv.LoopListen();});
    msleep(50);
    srv.LoopListenStop();
    EXPECT_EQ (SocketResult::SR_SUCCESS, res);
    if (th.joinable()) th.join();
}



TEST(SocketTCP, Transmission_Echo)
{
    TestServer<SocketTCP, TestServerClient<SocketTCP>> srv("TestServerGTest");
    TestClient<SocketTCP>                              cln("TestClient");

    srv.Port(5000);
    cln.Port(5000);

    SocketResult resSrv, resCln;

    EXPECT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    EXPECT_EQ (SocketResult::SR_SUCCESS, cln.Init());

    std::thread thSrv ( [&srv, &resSrv]() {resSrv = srv.LoopListen();} );


    EXPECT_EQ (SocketResult::SR_SUCCESS, cln.Connect());
    msleep(1);
    EXPECT_EQ (1, srv.ClientCount());

    std::thread thCln ( [&cln, &resCln]() {resCln = cln.LoopRead();  } );

    const char *p = "TestContent";
    EXPECT_EQ (SocketResult::SR_SUCCESS, cln.Send(p, std::strlen(p)));
    msleep(5);

    ASSERT_STREQ (p, srv.TestData(0).c_str());
    ASSERT_STREQ ( (std::string(p) + "::From-TestServerClient").c_str(), cln._testContent.c_str());


    srv.LoopListenStop();
    cln.LoopReadStop();

    if (thSrv.joinable()) thSrv.join();
    if (thCln.joinable()) thCln.join();

    EXPECT_EQ (SocketResult::SR_SUCCESS, resSrv);
    EXPECT_EQ (SocketResult::SR_SUCCESS, resCln);
}


TEST(SocketTCP, Transmission_MultipleConnect1)
{
    SocketResult resSrv, resCln, res;

    TestServer<SocketTCP, TestServerClient<SocketTCP>> srv("TestServerGTest");
    srv.Port(5000);
    EXPECT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    std::thread thSrv ( [&srv, &resSrv]() {resSrv = srv.LoopListen();} );

    const int countClient = 500;

    TestClient<SocketTCP> cln[countClient];

    res = SocketResult::SR_SUCCESS;
    const char *namePrefix = "TestClient-";

    for(int i = 0; i < countClient && (SocketResult::SR_SUCCESS == res); ++i)
    {
        cln[i].Name(std::string(namePrefix) += std::to_string(i));
        cln[i].Port(5000);
        EXPECT_EQ (SocketResult::SR_SUCCESS, res = cln[i].Init());
        EXPECT_EQ (SocketResult::SR_SUCCESS, res = cln[i].Connect());
    }

    msleep(100);
    EXPECT_EQ (countClient, srv.ClientCount());

    srv.Disconnect();

    srv.LoopListenStop();
    if (thSrv.joinable()) thSrv.join();
}


TEST(SocketTCP, Transmission_MultipleConnect_ClientLost)
{
    SocketResult resSrv, resCln, res;

    TestServer<SocketTCP, TestServerClient<SocketTCP>> srv("TestServerGTest");
    srv.Port(5000);
    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    std::thread thSrv ( [&srv, &resSrv]() {resSrv = srv.LoopListen();} );

    const int countClient = 250;

    //std::vector <std::thread> thClns;
    TestClient<SocketTCP> clns[countClient];


    for (int i = 0; i < 10; ++i)
    {
        for(int cidx = 0; cidx < countClient; ++cidx)
        {
            EXPECT_EQ (SocketResult::SR_SUCCESS, res = clns[cidx].Init());
            clns[cidx].Port(5000);
            EXPECT_EQ (SocketResult::SR_SUCCESS, res = clns[cidx].Connect());
        }
        msleep(1);
        EXPECT_EQ (countClient, srv.ClientCount());
        for(int cidx = 0; cidx < countClient; ++cidx)
        {
            clns[cidx].Release();
        }
        msleep(10);
        EXPECT_EQ (0, srv.ClientCount());
    }

    srv.LoopListenStop();
    if (thSrv.joinable()) thSrv.join();
}


TEST(SocketTCP, Transmission_MultipleConnect_ResetServer)
{
    SocketResult resSrv, resCln, res;

    TestServer<SocketTCP, TestServerClient<SocketTCP>> srv("TestServerGTest");
    srv.Port(5000);
    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    std::thread thSrv ( [&srv, &resSrv]() {resSrv = srv.LoopListen();} );

    const int countClient = 250;

    std::vector <std::thread> thClns;

    TestClient<SocketTCP> clns[countClient];

    for (int i = 0; i < 10; ++i)
    {
        for(int cidx = 0; cidx < countClient; ++cidx)
        {
            EXPECT_EQ (SocketResult::SR_SUCCESS, res = clns[cidx].Init());
            clns[cidx].Port(5000);
            EXPECT_EQ (SocketResult::SR_SUCCESS, res = clns[cidx].Connect());
        }
        msleep(1);
        EXPECT_EQ (countClient, srv.ClientCount());

        srv.Disconnect(); //All connections
        EXPECT_EQ (0, srv.ClientCount());
    }

    srv.LoopListenStop();
    if (thSrv.joinable()) thSrv.join();
}


TEST(SocketTCP, Transmission_MultipleEcho)
{
    SocketResult resSrv;
    TestServer<SocketTCP, TestServerClient<SocketTCP>> srv("TestServerGTest");
    srv.Port(5000);
    ASSERT_EQ (SocketResult::SR_SUCCESS, srv.Init());
    std::thread thSrv ( [&srv, &resSrv]() {resSrv = srv.LoopListen();} );


    const int countClient = 250;
    const std::string dataPrefix = "TestContent::From-TestClient";

    TestClient<SocketTCP> clns[countClient];
    SocketResult          clnsRes[countClient];
    std::thread           clnsTh[countClient];


    for(int cid = 0; cid < countClient; ++cid)
    {
        EXPECT_EQ (SocketResult::SR_SUCCESS, clns[cid].Init());
        clns[cid].Port(5000);
        EXPECT_EQ (SocketResult::SR_SUCCESS, clns[cid].Connect());

        clnsTh[cid] = std::thread ( [&clns, &cid, &clnsRes]() {clnsRes[cid] = clns[cid].LoopRead();  } );
        msleep(2);

        std::string data = dataPrefix + "." + std::to_string(cid);
        EXPECT_EQ (SocketResult::SR_SUCCESS, clns[cid].Send(data.c_str(), data.size()));
    }

    msleep(5);
    EXPECT_EQ (countClient, srv.ClientCount());

    for(int cid = 0; cid < countClient; ++cid)
    {
        std::string data = dataPrefix + "." + std::to_string(cid) + "::From-TestServerClient";
        EXPECT_STREQ (data.c_str(), clns[cid]._testContent.c_str());
    }

    srv.Disconnect();
    msleep(1);

    for(int cid = 0; cid < countClient; ++cid)
        clnsTh[cid].join();
    srv.LoopListenStop();
    if (thSrv.joinable()) thSrv.join();
}


