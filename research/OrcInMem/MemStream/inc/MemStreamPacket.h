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

#include <map>
#include <vector>


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
            "CmdID": "Write",
            "RefID": "UUID",
            "Status": "SUCCESS",
            "Message": "Command executed successfully"
        },
        {
            "Sender": "NodaAAA",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "Read",
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
            "CmdID": "Read",
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
            "CmdID": "Length",
            "RefID": "UUID",
            "Status": "REQUEST",
            "Message": ""
        },
        {
            "Sender": "NodaBBB",
            "Timestamp":"123123",
            "Source": "TableXXX",
            "CmdID": "Length",
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
        CMD_NA,
        CMD_LENGTH,
        CMD_WRITE,
        CMD_READ
    };
    enum class CmdStatus
    {
        CST_REQUEST,
        CST_SUCCESS,
        CST_ERROR
    };

    struct Cmd
    {
        CmdID       _cmdid  = CmdID::CMD_NA;
        std::string _cmdidStr  ;
        std::string _source ;
        std::string _sender ;
        std::string _ts     ;
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
    };


    MemStreamPacket();
    MemStreamPacket(const MemStreamPacket& orig)    = default;
    MemStreamPacket(MemStreamPacket&& orig)         = default;

    virtual ~MemStreamPacket();

    void            JsonBase            (rapidjson::Value &item, rapidjson::Document::AllocatorType &a, CmdID cmd, const char *source);
    void            JsonDataB64         (rapidjson::Value &item, rapidjson::Document::AllocatorType &a, std::size_t length, std::size_t offset = 0, const void *data = nullptr);
    std::uint32_t   CreatePacketWrite   (const char *source, const void *data, std::size_t len);
    std::uint64_t   CreateRefID();

    int             DecodePacket        (const StreamPacket &, std::vector<Cmd> &cmds);


private:
    std::map<CmdID, std::string> _strCmd       = { { CmdID::CMD_LENGTH,      "LENGTH"  },
                                                   { CmdID::CMD_WRITE,       "WRITE"   },
                                                   { CmdID::CMD_READ,        "READ"    }
    };
    std::map<CmdStatus, std::string> _strCStat = { { CmdStatus::CST_REQUEST, "REQUEST" },
                                                   { CmdStatus::CST_SUCCESS, "SUCCESS" },
                                                   { CmdStatus::CST_ERROR,   "ERROR"   }
    };
};

#endif /* __ORC_STREAM_REMOTE_PACKET_H__ */

