/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SocketServerPacket.h
 * Author: can.gursu
 *
 * Created on 19 July 2018, 15:22
 */

#ifndef __SOURCE_CHANNEL_H__
#define __SOURCE_CHANNEL_H__

#include "SocketServer.h"
#include "SocketClient.h"
#include "SocketUtils.h"
#include "StreamPacket.h"
#include "LoggerGlobal.h"


#include <thread>



template <typename TQueue, typename TSocketSrv, typename TSocketClt>
class SocketServerPacket : public SocketServer <TSocketSrv, TSocketClt>
{
    using byte_t  = StreamPacket::byte_t;
    using msize_t = StreamPacket::msize_t;

    public:
        struct Prm
        {
            std::string  _sourceName;
            std::string  _logName;
            TQueue      &_que;
        };

        SocketServerPacket(const SocketServerPacket::Prm prm)
            : SocketServer<TSocketSrv, TSocketClt>("SocketServerPacket")
            , _prm(prm)
        {
            LogLineGlbSocketName(_prm._logName.c_str());
            SocketServer <TSocketSrv, TSocketClt>::SocketPath(_prm._sourceName.c_str());

            std::cout << "Using Source : " << _prm._sourceName << std::endl;
            std::cout << "Using Logger : " << _prm._logName    << std::endl;

            LOG_LINE_GLOBAL("SServerClient", SocketServer<TSocketSrv, TSocketClt>::Name(), " - Using Source : ", _prm._sourceName);
            LOG_LINE_GLOBAL("SServerClient", SocketServer<TSocketSrv, TSocketClt>::Name(), " - Using Logger : ", _prm._logName);
        }

        virtual void OnAccept(const TSocketClt &sock, const sockaddr &addr)
        {
            std::string host, serv;
            if (true == NameInfo(addr, host, serv))
            {
                LOG_LINE_GLOBAL("SServerClient", "Accepted connection on host = ", host, " port = ", serv);
                ClientCount();
            }
        }

        virtual void OnDisconnect  (const TSocketClt &sock)
        {
            LOG_LINE_GLOBAL("SServerClient", "Client Disconnected.");
            ClientCount();
        }

        virtual void OnErrorClient(SocketResult res)
        {
            LOG_LINE_GLOBAL("SServerClient", "ErrorClient : ", SocketResultText(res));
        }

        virtual void OnErrorServer(SocketResult res)
        {
            LOG_LINE_GLOBAL("SServerClient", "ErrorServer : ", SocketResultText(res));
        }

        void ClientCount()
        {
            LOG_LINE_GLOBAL("SServerClient", "Client count = ", SocketServer<TSocketSrv, TSocketClt>::ClientCount());
        }

    private:
        Prm _prm;
};




#endif // __SOURCE_CHANNEL_H__



