/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcRemoteServerClient.cpp
 * Author: can.gursu
 *
 * Created on 17 October 2018, 11:07
 */

#include "OrcRemoteServerClient.h"
#include "libbase64.h"



int OrcRemoteServerClient::DecodePacket(StreamPacket &packet, std::vector <MemStreamPacket::Cmd> &cmds)
{
    int res = -1;
    if (_pstore)
    {
        MemStreamPacket stream;
        res = stream.DecodePacket(packet, cmds);
    }
    return res;
}


void OrcRemoteServerClient::DisplayCommad(const MemStreamPacket::Cmd &cmd)
{
    std::time_t tmt = std::strtol(cmd._ts.c_str(), nullptr, 10);
    std::cout << cmd.Dump() << std::endl;

    size_t outLen = 0;
    try
    {
        std::strtol(cmd._buffer._length.c_str(), nullptr, 10);
    }
    catch(...)
    {
    }

    char outBuff[outLen];
    if (outLen > 0)
    {
        const char *ppp = &cmd._buffer._buf[0];
        base64_decode (ppp, cmd._buffer._buf.size(), outBuff, &outLen, BASE64_FORCE_AVX2);
    }

    std::cout << "Decoded outLen  : " << outLen                       << std::endl;
    std::cout << "Decoded outBuff : " << std::string(outBuff, outLen) << std::endl;
    std::cout << std::endl;
}

void OrcRemoteServerClient::DisplayPacket(StreamPacket &packet)
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



void OrcRemoteServerClient::OnRecvPacket(StreamPacket &&packet)
{
    std::vector <MemStreamPacket::Cmd> cmds;
    if ( 0 > DecodePacket(packet, cmds))
    {
        OnErrorClient(SocketResult::SR_ERROR_PARSE);
    }
    else
    {
        for(const auto &cmd : cmds)
        {
            //DisplayCommad(cmd);
            switch (cmd._cmdid)
            {
                case MemStreamPacket::CmdID::CMD_WRITE      :   OnRecvCmdWrite(cmd);  break;
                case MemStreamPacket::CmdID::CMD_READ       :   OnRecvCmdRead(cmd);   break;
                case MemStreamPacket::CmdID::CMD_DELETE     :   OnRecvCmdDelete(cmd); break;
                case MemStreamPacket::CmdID::CMD_LENGTH     :   OnRecvCmdLength(cmd); break;
                    break;
                default                                     :
                    break;
            }
        }
    }
}

void OrcRemoteServerClient::OnRecvCmdWrite(const MemStreamPacket::Cmd &cmd)
{
    MemStreamPacket pckResponse;
    if (nullptr == _pstore)
    {
        pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_ERROR, "Server Not Ready");
    }
    else
    {
        size_t buffLen = cmd._buffer._buf.size();
        char buff[buffLen] = "";
        base64_decode (&cmd._buffer._buf[0], cmd._buffer._buf.size(), buff, &buffLen, BASE64_FORCE_AVX2);

        //std::cout << "Write: _source:" << cmd._source << " buff:" << std::string(buff, buffLen) << std::endl;
        _pstore->Write(cmd._source.c_str(), buff, buffLen);
        pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_SUCCESS, "Write Suceeded");
    }

    SendPacket(pckResponse);
}

void OrcRemoteServerClient::OnRecvCmdRead(const MemStreamPacket::Cmd &cmd)
{
    MemStreamPacket pckResponse;
    if (nullptr == _pstore)
    {
        pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_ERROR, "Server Not Ready");
    }
    else
    {
        char buff[std::strtol(cmd._buffer._length.c_str(), nullptr, 10)] = "";
        std::uint64_t lenrd = _pstore->Read(cmd._source.c_str(),
                                            (void*)buff, std::strtol(cmd._buffer._length.c_str(), nullptr, 10),
                                            std::strtol(cmd._buffer._offset.c_str(), nullptr, 10));
        //std::cout << "Read: _source:" << cmd._source << " buff:" << std::string(buff, lenrd) << std::endl;

        if ((lenrd < 0) || (lenrd == (std::uint64_t)-1))
        {
            pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_ERROR, "Source Not Found");
        }
        else
        {
            pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_SUCCESS, "Reading Suceeded", lenrd, buff);
        }
    }
    SendPacket(pckResponse);
}

void OrcRemoteServerClient::OnRecvCmdDelete(const MemStreamPacket::Cmd &cmd)
{
    MemStreamPacket pckResponse;
    if (nullptr == _pstore)
    {
        pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_ERROR, "Server Not Ready");
    }
    else
    {
        if (((std::uint64_t)-1) == _pstore->Delete(cmd._source.c_str()))
            pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_ERROR, "Source Not Found");
        else
            pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_SUCCESS, "Deletion Suceeded");
    }
    SendPacket(pckResponse);
}

void OrcRemoteServerClient::OnRecvCmdLength(const MemStreamPacket::Cmd &cmd)
{
    MemStreamPacket pckResponse;
    if (nullptr == _pstore)
    {
        pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_ERROR, "Server Not Ready");
    }
    else
    {
        std::uint64_t len = _pstore->Len(cmd._source.c_str());
        if (len == ((std::uint64_t)-1))
            pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_ERROR, "Source Not Found");
        else
            pckResponse.CreatePacketResponse("OrcRemoteServerClient", cmd, MemStreamPacket::CmdStatus::CST_SUCCESS, "Length got successfully", len);
    }
    SendPacket(pckResponse);
}




