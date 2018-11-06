/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcRemoteServer.h
 * Author: can.gursu
 *
 * Created on 17 October 2018, 11:08
 */

#ifndef __ORC_REMOTE_SERVER_H__
#define __ORC_REMOTE_SERVER_H__


#include "SocketServer.h"
#include "StreamPacket.h"
#include "MemStreamPacketStore.h"




template <typename TSockSrv, typename TSockCln>
class OrcRemoteServer : public SocketServer<TSockSrv, TSockCln>
{
    using  msize_t = StreamPacket::msize_t;

    public:

        OrcRemoteServer(const char *name):SocketServer<TSockSrv, TSockCln>(name)
        {
        }


        virtual void OnAccept(TSockCln &sock, const sockaddr &addr)
        {
            sock.SetStore(&_storeInstant);
            std::cout << "Accepted connection - ";
            PrintClientCount();
        }
/*
        virtual void OnRecv(TSockCln &sock, MemStream<std::uint8_t> &&stream)
        {
            //std::cout << stream.dump("OrcRemoteServer::OnRecv");
            sock.OnRecv(std::move(stream));
        }
*/
        virtual void OnDisconnect  (const TSockCln &sock)
        {
            std::cout << "Client Disconnected - ";
            PrintClientCount();
        }

        virtual void OnErrorClient(const TSockCln &cln, SocketResult res)
        {
            std::cout << "ErrorClient : " << SocketResultText(res) << std::endl;
        }

        virtual void OnErrorServer(SocketResult res)
        {
            std::cout << "ErrorServer : " << SocketResultText(res) << std::endl;
        };

        void PrintClientCount()
        {
            std::cout << "Client count = " << SocketServer<TSockSrv, TSockCln>::ClientCount() << std::endl;
        }

    private:
        MemStreamPacketMap _storeInstant;
};





#endif /* __ORC_REMOTE_SERVER_H__ */

