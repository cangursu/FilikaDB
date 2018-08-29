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

        virtual void OnRecv(/*const*/ TSocketClt &sock, MemStream<uint8_t> &&stream)
        {
            sock.OnRecv(std::move(stream));
            /*
            msize_t offsetStream = 0L;
            msize_t offsetPacket = 0L;
            msize_t stLen        = stream.Len();


            LOG_LINE_GLOBAL("SServerClient");

            SocketResult res = SocketResult::SR_ERROR_AGAIN;
            while(SocketResult::SR_ERROR_AGAIN == res)
            {
                StreamPacket packet;
                auto reader = [&stream, &offsetStream] (char *buff, int len) -> int { return stream.read(buff, len, offsetStream); };
                res = sock.recvPacket(packet, reader);
                //res = sock.recvPacket(packet, stream, offsetStream);

                //LOG_LINE_GLOBAL("SServerClient", "recvPAcket : ", SocketResultText(res));
                if (res == SocketResult::SR_ERROR_AGAIN || res == SocketResult::SR_SUCCESS)
                {
                    offsetStream += packet.BufferLen();
                    _prm._que.push(std::move(packet));

                    //msize_t stLen        = stream.Len();
                    //msize_t pkLen = packet.BufferLen();
                    //msize_t pyLen = packet.PayloadLen();
                    //LOG_LINE_GLOBAL("SServerClient", "stLen:", stLen, " pkLen:", pkLen, ", pyLen:", pyLen, ", offsetStream:", offsetStream);
                    //const msize_t        bufferLen = 128;
                    //StreamPacket::byte_t buffer [bufferLen];
                    //for (msize_t i = 0; i < pyLen; i += bufferLen)
                    //{
                    //    if (packet.PayloadPart(buffer, bufferLen, i) > 0)
                    //    {
                    //        LOG_LINE_GLOBAL("SServerClient", "Packet:", std::string((char*)buffer, pyLen));
                    //    }
                    //}
                }
            }
        */
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



