/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcRemoteServerClient.h
 * Author: can.gursu
 *
 * Created on 17 October 2018, 11:07
 */

#ifndef __ORC_REMOTE_SERVER_CLIENT_H__
#define __ORC_REMOTE_SERVER_CLIENT_H__

#include "SocketClientPacket.h"
#include "SocketTCP.h"
#include "MemStreamPacketStore.h"
#include "MemStreamPacket.h"

#include <ctime>


class OrcRemoteServerClient : public SocketClientPacket<SocketTCP>
{
public:
    OrcRemoteServerClient(const char *name = "OrcRemoteServerClient")
            : SocketClientPacket(name)
    {
    }

    OrcRemoteServerClient(int fd, const char *name)
        : SocketClientPacket(fd, name)
    {
    }

    virtual void OnErrorClient (SocketResult err)
    {
        std::cerr << "Error : OrcRemoteServerClient::OnErrorClient - " << Name() << " : " << SocketResultText(err) << std::endl;
    }

    void SetStore(MemStreamPacketMap *store)
    {
        _pstore = store;
    }

    void            OnRecvCmdWrite  (const MemStreamPacket::Cmd &cmd);
    void            OnRecvCmdRead   (const MemStreamPacket::Cmd &cmd);
    void            OnRecvCmdDelete (const MemStreamPacket::Cmd &cmd);
    void            OnRecvCmdLength (const MemStreamPacket::Cmd &cmd);


    virtual void    OnRecvPacket    (StreamPacket &&packet);
    int             DecodePacket    (StreamPacket &packet, std::vector <MemStreamPacket::Cmd> &cmds);
    void            DisplayCommad   (const MemStreamPacket::Cmd &cmd);
    void            DisplayPacket   (StreamPacket &packet);

    MemStreamPacketMap *_pstore = nullptr;
};

#endif /* __ORC_REMOTE_SERVER_CLIENT_H__ */

