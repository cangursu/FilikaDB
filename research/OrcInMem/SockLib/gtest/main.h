/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.h
 * Author: can.gursu
 *
 * Created on 10 September 2018, 09:59
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "SocketServer.h"
#include "SocketClient.h"
#include "MemStream.h"

int msleep(long miliseconds);
int nsleep(long nanoseconds);

std::string MemStream2String(MemStream<std::uint8_t> &stream);



template <typename TSock>
class TestServerClient : public SocketClient<TSock>
{
    public:
        TestServerClient(const char *name = "TestServerClient")
            : SocketClient<TSock>(name)
        {
        }
        TestServerClient(int fd, const char *name)
            : SocketClient<TSock>(fd, name)
        {
        }
        virtual void OnRecv (MemStream<std::uint8_t> &&stream)
        {
            _testContent = MemStream2String(stream);

            const void* buf = "::From-TestServerClient";
            stream.write(buf, std::strlen((char*)buf));

            std::string str = MemStream2String(stream);

            if (SocketResult::SR_SUCCESS != this->Send(stream))
            {
                std::cerr << "ERRORRRR......\n";
            }
        }
        virtual void OnErrorClient (SocketResult res)
        {
            std::cout << "ErrorClient::OnErrorClient - " << SocketResultText(res) << std::endl;
        }

        std::string  _testContent = "###Empty###_TestServerClient";
        //bool         _doEcho      = true;
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
            //std::cout << "Connection Accepted.  _conCount = " << ++_conCount << " - ";
            //ClientCountMsg();
        }
        virtual void OnDisconnect  (const TSockCln &sock)
        {
            //std::cout << "Client Disconnected. _conCount = " << _conCount << " - ";
            //ClientCountMsg();
        }
        virtual void OnErrorClient(const TSockCln &cln, SocketResult res)
        {
            std::cout << "ErrorClient : Name:" << (cln.Name()) << " - " << "Result:" << SocketResultText(res) << " - errno: (" << ErrnoText(errno) << ") " << strerror(errno) << std::endl;
            this->Disconnect(cln);
        }
        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << " - errno : (" << ErrnoText(errno) << ") " << strerror(errno) << std::endl;
        };
        void ClientCountMsg()
        {
            std::cout << "Client count = " << this->ClientCount() << std::endl;
        }

        std::string TestData(int idx)
        {
            const TSockCln *pp = this->ClientByIdx(idx);
            if (pp)
                return pp->_testContent;
            return "";
        }

        int _conCount = 0;

};



template <typename TSock>
class TestClient : public SocketClient<TSock>
{
    public:
        TestClient(const char *name = "TestClient") : SocketClient<TSock>(name) {}

        virtual void OnRecv(MemStream<std::uint8_t> &&stream)
        {
            _testContent = MemStream2String(stream);

            //std::cout << "TestClient::OnRecv -> " << stream.Len() << ":" << _testContent << std::endl;
        }
        virtual void OnErrorClient(SocketResult res)
        {
            std::cout << "ERROR  : " << SocketResultText(res) << " - (" << errno <<  ") " << strerror(errno) << " - " << this->fd() << std::endl;
        }
        std::string  _testContent = "###Empty###_TestClient";
};




#endif /* __MAIN_H__ */

