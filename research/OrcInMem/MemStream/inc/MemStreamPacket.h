/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MemStreamPacket.h
 * Author: can.gursu
 *
 * Created on 27 September 2018, 11:07
 */

#ifndef __ORC_STREAM_REMOTE_PACKET_H__
#define __ORC_STREAM_REMOTE_PACKET_H__

#include "StreamPacket.h"
#include "rapidjson/document.h"
#include "libbase64.h"



#include <uuid/uuid.h>
#include <map>
#include <vector>
#include <ctime>


/*


{
    "MYSFRIF":
    [
        {
            "Sender": "NodaAAA",
            "Timestamp": "123123",
            "Source": "TableXXX",
            "CmdID": "WRITE",
            "RefID": "UUID",
            "Status": "REQUEST",
            "Message": "",
            "Buffer":
            {
                "Content": "0234423",
                "Lenght": "234",
                "Offset": "234",
                "Enc": "base64"
            }
        }
        ,
        {
            "Sender": "NodaBBB",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "WRITE",
            "RefID": "UUID",
            "Status": "SUCCESS",
            "Message": "Command executed successfully"
        },
        {
            "Sender": "NodaAAA",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "READ",
            "RefID": "UUID",
            "Status": "REQUEST",
            "Message": "Command executed successfully",
            "Buffer":
            {
                "Lenght": "234",
                "Offset": "234"
            }
        },
        {
            "Sender": "NodaBBB",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "READ",
            "RefID": "UUID",
            "Status": "SUCCESS",
            "Message": "",
            "Buffer":
            {
                "Content": "0234423",
                "Lenght": "234",
                "Enc": "base64"
            }
        },
        {
            "Sender": "NodaAAA",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "DELETE",
            "RefID": "UUID",
            "Status": "REQUEST",
            "Message": ""
        },
        {
            "Sender": "NodaBBB",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "DELETE",
            "RefID": "UUID",
            "Status": "ERROR",
            "Message": "Unable to execute request",
        }
        {
            "Sender": "NodaAAA",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "LENGTH",
            "RefID": "UUID",
            "Status": "REQUEST",
            "Message": ""
        },
        {
            "Sender": "NodaBBB",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "LENGTH",
            "RefID": "UUID",
            "Status": "ERROR",
            "Message": "Unable to execute request",
            "Buffer":
            {
                "Lenght": "234"
            }
        }
    ]
}


 */



class MemStreamPacket : public StreamPacket
{
public:

    enum class CmdID
    {
        CMD_NA = 0,
        CMD_LENGTH,
        CMD_WRITE,
        CMD_READ,
        CMD_DELETE,
    };
    enum class CmdStatus
    {
        CST_REQUEST,
        CST_SUCCESS,
        CST_ERROR
    };

    struct Cmd
    {
        std::string _sender  ;
        std::string _ts      ;
        std::string _source  ;
        CmdID       _cmdid   = CmdID::CMD_NA;
        std::string _cmdidStr;
        std::string _refid  ;
        std::string _result ;
        std::string _message;
        struct buffer_t
        {
            std::string       _length;
            std::string       _offset;
            std::string       _enc;
            std::vector<char> _buf;
        } _buffer;

        MemStream<StreamPacket::byte_t> DecodeBuffer() const;
        std::string                     Dump() const;
    };


    MemStreamPacket();
    MemStreamPacket(const MemStreamPacket& orig)    = default;
    MemStreamPacket(MemStreamPacket&& orig)         = default;

    virtual ~MemStreamPacket();

    static void     JsonBase                (rapidjson::Value &item,
                                             rapidjson::Document::AllocatorType &a,
                                             CmdID cmd,
                                             const char *refID,
                                             CmdStatus cmdStat,
                                             const char *sender,
                                             const char *source,
                                             const char *msg = nullptr);

    static void     JsonDataB64             (rapidjson::Value &item,
                                             rapidjson::Document::AllocatorType &a,
                                             std::size_t length,
                                             std::size_t offset = 0,
                                             const void *data = nullptr);

    static void     JsonDataB64             (rapidjson::Value &item,
                                             rapidjson::Document::AllocatorType &a,
                                             std::size_t length,
                                             std::size_t offset,
                                             MemStream<byte_t> &stream);

    std::string     CreatePacketWrite       (const char *sender, const char *source, std::size_t len, MemStream<StreamPacket::byte_t> &stream);
    std::string     CreatePacketWrite       (const char *sender, const char *source, std::size_t len, const void *data);
    std::string     CreatePacketRead        (const char *sender, const char *source, std::size_t len, std::size_t offset);
    std::string     CreatePacketDelete      (const char *sender, const char *source);
    std::string     CreatePacketLength      (const char *sender, const char *source);
    std::string     CreatePacketResponse    (const char *sender, const Cmd &cmd, CmdStatus stat, const char *msg, std::size_t len = 0, const void *data = nullptr);

    virtual std::string CreateRefID         ();

    int                 DecodePacket        (const StreamPacket &, std::vector<Cmd> &cmds);


    static const std::string&  CmdIDText           (CmdID cmdid)            { return _strCmd[cmdid]; }
    static CmdID               CmdIDVal            (const std::string &str)
    {
        for(const auto &scmd : _strCmd)
            if (scmd.second == str) return scmd.first;
        return CmdID::CMD_NA;
    }

private:
    static std::map<CmdID,     std::string>     _strCmd;
    static std::map<CmdStatus, std::string>     _strCStat;
};

#endif /* __ORC_STREAM_REMOTE_PACKET_H__ */

