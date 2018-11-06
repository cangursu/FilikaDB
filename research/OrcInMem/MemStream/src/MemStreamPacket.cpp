/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MemStreamPacket.cpp
 * Author: can.gursu
 *
 * Created on 27 September 2018, 11:07
 */

#include "MemStreamPacket.h"
#include "MemStream.h"
#include "GeneralUtils.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "libbase64.h"

#include <ctime>
#include <vector>

std::map<MemStreamPacket::CmdID, std::string> MemStreamPacket::_strCmd       =
{
    { CmdID::CMD_LENGTH,      "LENGTH"   },
    { CmdID::CMD_WRITE,       "WRITE"    },
    { CmdID::CMD_READ,        "READ"     },
    { CmdID::CMD_DELETE,      "DELETE"   }
};

std::map<MemStreamPacket::CmdStatus, std::string> MemStreamPacket::_strCStat =
{
    { CmdStatus::CST_REQUEST, "REQUEST"  },
    { CmdStatus::CST_SUCCESS, "SUCCESS"  },
    { CmdStatus::CST_ERROR,   "ERROR"    }
};



MemStream<StreamPacket::byte_t> MemStreamPacket::Cmd::DecodeBuffer() const
{
    MemStream<StreamPacket::byte_t> dbuff;
    uint64_t length = std::strtol(_buffer._length.c_str(), nullptr, 10);

    if (length > 0)
    {
        if (_buffer._enc == "Base64")
        {
            size_t buffLen = _buffer._buf.size();
            char   buff[buffLen] = "";
            base64_decode (&_buffer._buf[0], _buffer._buf.size(), buff, &buffLen, BASE64_FORCE_AVX2);
            dbuff.Write(buff, buffLen);
        }
        else
        {
            std::cerr << "ERROR : Unknown Packet Encoding\n";
        }
    }
    return std::move(dbuff);
}

std::string MemStreamPacket::Cmd::Dump() const
{
    time_t tmt = std::strtol(_ts.c_str(), nullptr, 10);

    std::stringstream ss;
    ss << "\nCommand \n";
    ss << "_sender         : " << _sender         << std::endl;
    ss << "_ts             : " << std::ctime(&tmt);
    ss << "_source         : " << _source         << std::endl;
    ss << "_cmdid          : " << _cmdidStr       << std::endl;
    ss << "_refid          : " << _refid          << std::endl;
    ss << "_result         : " << _result         << std::endl;
    ss << "_message        : " << _message        << std::endl;
    ss << "_buffer._length : " << _buffer._length << std::endl;
    ss << "_buffer._offset : " << _buffer._offset << std::endl;
    ss << "_buffer._enc    : " << _buffer._enc    << std::endl;
    ss << "_buffer._buf    : " << std::string(&_buffer._buf[0], _buffer._buf.size()) << std::endl;


    MemStream<StreamPacket::byte_t> dbuff = DecodeBuffer();
    ss << dbuff.Dump("Decoded Buffer");

    return ss.str();
}





MemStreamPacket::MemStreamPacket()
{
}


MemStreamPacket::~MemStreamPacket()
{
}

std::string MemStreamPacket::CreateRefID()
{
    uuid_t uuid;
    uuid_generate(uuid);

    char buff[64] = "";
    uuid_unparse(uuid, buff);

    return std::move(std::string(buff));
}


void MemStreamPacket::JsonDataB64(rapidjson::Value &cmdItem,
                                  rapidjson::Document::AllocatorType &a,
                                  std::size_t length,
                                  std::size_t offset,
                                  MemStream<byte_t> &data)
{
    rapidjson::Value buffItem;
    buffItem.SetObject();

    const std::uint64_t dataLen   = data.Len();
    std::uint64_t       data64Len = int(dataLen * 4.0 / 3.0) + 16;

    if (0 == dataLen)
    {
        buffItem.AddMember("Lenght" , rapidjson::Value (std::to_string(length).c_str() , a), a);
        buffItem.AddMember("Offset" , rapidjson::Value (std::to_string(offset).c_str() , a), a);
    }
    else
    {
        char data64[data64Len] = "";
        EncodeBase64(data, data64, data64Len);

        buffItem.AddMember("Lenght" , rapidjson::Value (std::to_string(data64Len).c_str() , a), a);
        buffItem.AddMember("Enc"    , "Base64", a);
        buffItem.AddMember("Content", rapidjson::Value (data64, data64Len, a)  , a);
    }
    cmdItem.AddMember("Buffer", buffItem, a);

}

void MemStreamPacket::JsonDataB64(rapidjson::Value &cmdItem,
                                  rapidjson::Document::AllocatorType &a,
                                  std::size_t length,
                                  std::size_t offset /*= 0*/,
                                  const void *data /*= nullptr*/)
{
    rapidjson::Value buffItem;
    buffItem.SetObject();

    if (nullptr == data)
    {
        buffItem.AddMember("Lenght" , rapidjson::Value (std::to_string(length).c_str() , a), a);
        buffItem.AddMember("Offset" , rapidjson::Value (std::to_string(offset).c_str() , a), a);
    }
    else
    {
        size_t outlen = (4.0*length/3.0)+2;
        char out[outlen] = "";
        base64_encode((char*)data, length, out, &outlen, BASE64_FORCE_AVX2);

        buffItem.AddMember("Lenght" , rapidjson::Value (std::to_string(outlen).c_str() , a), a);
        buffItem.AddMember("Enc"    , "Base64"                         , a);
        buffItem.AddMember("Content", rapidjson::Value (out, outlen, a), a);
    }
    cmdItem.AddMember("Buffer", buffItem, a);
}

void MemStreamPacket::JsonBase(rapidjson::Value &cmdItem,
                               rapidjson::Document::AllocatorType &a,
                               CmdID cmd,
                               const char *refID,
                               CmdStatus cmdStat,
                               const char *sender,
                               const char *source,
                               const char *msg /* = nullptr*/)
{
    std::time_t tmeSmp = std::time(nullptr);

    cmdItem.AddMember("Sender",    rapidjson::Value(sender                                , a)  , a);
    cmdItem.AddMember("Timestamp", rapidjson::Value(std::to_string(tmeSmp).c_str()        , a)  , a);
    cmdItem.AddMember("Source",    rapidjson::Value(source                                , a)  , a);
    cmdItem.AddMember("CmdID",     rapidjson::Value(_strCmd[cmd].c_str()                  , a)  , a);
    cmdItem.AddMember("RefID",     rapidjson::Value(refID                                 , a)  , a);

    cmdItem.AddMember("Status",    rapidjson::Value(_strCStat[cmdStat].c_str()            , a)  , a);
    if(msg && *msg)
        cmdItem.AddMember("Message",    rapidjson::Value(msg , a)  , a);

}


std::string MemStreamPacket::CreatePacketRead(const char *sender, const char *source, std::size_t len, std::size_t offset)
{
    std::string refid = CreateRefID();

    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Document::AllocatorType &a = doc.GetAllocator();
    rapidjson::Value cmdItem;
    cmdItem.SetObject();

    MemStreamPacket::JsonBase(cmdItem, a, CmdID::CMD_READ, refid.c_str(), CmdStatus::CST_REQUEST, sender, source);
    MemStreamPacket::JsonDataB64(cmdItem, a, len, offset);

    rapidjson::Value cmdArray(rapidjson::kArrayType);
    cmdArray.PushBack(cmdItem, a);
    doc.AddMember("MYSFRIF", cmdArray, a);

    rapidjson::StringBuffer sbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sbuf);
    doc.Accept(writer);

    Reset();
    if (0 < StreamPacket::Create(sbuf.GetString(), sbuf.GetSize()))
        return refid;
    else
        return "";
}

std::string MemStreamPacket::CreatePacketWrite(const char *sender, const char *source, std::size_t len, MemStream<StreamPacket::byte_t> &stream)
{

}

std::string MemStreamPacket::CreatePacketWrite(const char *sender, const char *source, std::size_t len, const void *data)
{
    std::string refid = CreateRefID();

    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Document::AllocatorType &a = doc.GetAllocator();
    rapidjson::Value cmdItem;
    cmdItem.SetObject();

    MemStreamPacket::JsonBase(cmdItem, a, CmdID::CMD_WRITE, refid.c_str(), CmdStatus::CST_REQUEST, sender, source);
    MemStreamPacket::JsonDataB64(cmdItem, a, len, 0, data);


    rapidjson::Value cmdArray(rapidjson::kArrayType);
    cmdArray.PushBack(cmdItem, a);
    doc.AddMember("MYSFRIF", cmdArray, a);

    rapidjson::StringBuffer sbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sbuf);
    doc.Accept(writer);

    Reset();
    if (0 < StreamPacket::Create(sbuf.GetString(), sbuf.GetSize()))
        return refid;
    else
        return "";
}


std::string MemStreamPacket::CreatePacketDelete(const char *sender, const char *source)
{
    std::string refid = CreateRefID();

    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Document::AllocatorType &a = doc.GetAllocator();
    rapidjson::Value cmdItem;
    cmdItem.SetObject();

    JsonBase(cmdItem, a, CmdID::CMD_DELETE, refid.c_str(), CmdStatus::CST_REQUEST, sender, source);

    rapidjson::Value cmdArray(rapidjson::kArrayType);
    cmdArray.PushBack(cmdItem, a);
    doc.AddMember("MYSFRIF", cmdArray, a);

    rapidjson::StringBuffer sbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sbuf);
    doc.Accept(writer);

    Reset();
    if (0 < StreamPacket::Create(sbuf.GetString(), sbuf.GetSize()))
        return refid;
    else
        return "";
}


std::string MemStreamPacket::CreatePacketLength(const char *sender, const char *source)
{
    std::string refid = CreateRefID();

    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Document::AllocatorType &a = doc.GetAllocator();
    rapidjson::Value cmdItem;
    cmdItem.SetObject();

    JsonBase(cmdItem, a, CmdID::CMD_LENGTH, refid.c_str(), CmdStatus::CST_REQUEST, sender, source);

    rapidjson::Value cmdArray(rapidjson::kArrayType);
    cmdArray.PushBack(cmdItem, a);
    doc.AddMember("MYSFRIF", cmdArray, a);

    rapidjson::StringBuffer sbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sbuf);
    doc.Accept(writer);

    Reset();
    if (0 < StreamPacket::Create(sbuf.GetString(), sbuf.GetSize()))
        return refid;
    else
        return "";
}



std::string MemStreamPacket::CreatePacketResponse(const char *sender, const Cmd &cmd, CmdStatus stat, const char *msg, std::size_t len, const void *data)
{
    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Document::AllocatorType &a = doc.GetAllocator();
    rapidjson::Value cmdItem;
    cmdItem.SetObject();

    MemStreamPacket::JsonBase(cmdItem, a, cmd._cmdid, cmd._refid.c_str(), stat, sender, cmd._source.c_str(), msg);
    /*if (data) */MemStreamPacket::JsonDataB64(cmdItem, a, len, 0, data);

    rapidjson::Value cmdArray(rapidjson::kArrayType);
    cmdArray.PushBack(cmdItem, a);
    doc.AddMember("MYSFRIF", cmdArray, a);

    rapidjson::StringBuffer sbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sbuf);
    doc.Accept(writer);

    Reset();
    if (0 < StreamPacket::Create(sbuf.GetString(), sbuf.GetSize()))
        return cmd._refid;
    else
        return "";

}


int  MemStreamPacket::DecodePacket (const StreamPacket &packet, std::vector<Cmd> &cmds)
{
//    std::cout << packet.DumpPayload("MemStreamPacket::DecodePacket\n");

    MemStream<StreamPacket::byte_t> stream;
    if (0 < packet.Payload(stream))
    {
        class RJMemStream
        {
        public:
            RJMemStream(MemStream<StreamPacket::byte_t> &stream) : _stream(stream) {};
            typedef char Ch;
            Ch      Peek() const        { char ch; _stream.Read(&ch, 1, _pos); return ch; }
            Ch      Take()              { char ch = Peek(); ++_pos; return ch; }
            size_t  Tell()              { return _pos;    }

            Ch*     PutBegin()          { return nullptr; }
            void    Put(Ch)             {}
            void    Flush()             {}
            size_t  PutEnd(Ch* begin)   { return 0; }

            MemStream<StreamPacket::byte_t> &   _stream;
            int                                 _pos = 0;
        };

        RJMemStream rjs(stream);
        rapidjson::Document doc;
        rapidjson::ParseResult ok = doc.ParseStream(rjs);
        if (!ok) return -1;

        if (false == doc.IsObject() || false == doc.HasMember("MYSFRIF"))
            return -1;
        if(false == doc["MYSFRIF"].IsArray())
            return -1;

        cmds.empty(); //????

        for (auto& v : doc["MYSFRIF"].GetArray())
        {
            // Parameter Validation
            if (false == v.HasMember("CmdID")     || false == v["CmdID"    ].IsString()  )     break;
            if (false == v.HasMember("Source")    || false == v["Source"   ].IsString()  )     break;
            if (false == v.HasMember("Sender")    || false == v["Sender"   ].IsString()  )     break;
            if (false == v.HasMember("Timestamp") || false == v["Timestamp"].IsString()  )     break;
            if (false == v.HasMember("RefID")     || false == v["RefID"    ].IsString()  )     break;
            if (false == v.HasMember("Status")    || false == v["Status"   ].IsString()  )     break;

            Cmd cmd;
            cmd._cmdidStr = v["CmdID"].GetString();;
            cmd._source   = v["Source"].GetString();
            cmd._sender   = v["Sender"].GetString();
            cmd._ts       = v["Timestamp"].GetString();
            cmd._refid    = v["RefID"].GetString();
            cmd._result   = v["Status"].GetString();

            cmd._cmdid    = MemStreamPacket::CmdIDVal(cmd._cmdidStr);
            //for(const auto &s : _strCmd)
            //    if (s.second == cmd._cmdidStr) cmd._cmdid = s.first;

            if (v.HasMember("Message") && v["Message"].IsString()) cmd._message = v["Message"].GetString();
            if (v.HasMember("Buffer"))
            {
                auto &b = v["Buffer"];

                if (b.HasMember("Lenght") && b["Lenght"].IsString()) cmd._buffer._length = b["Lenght"].GetString();
                if (b.HasMember("Offset") && b["Offset"].IsString()) cmd._buffer._offset = b["Offset"].GetString();
                if (b.HasMember("Enc")    && b["Enc"].IsString())    cmd._buffer._enc    = b["Enc"].GetString();
                if (b.HasMember("Content"))
                {
                    class VectorHolder
                    {
                        public:
                            typedef char Ch;
                            VectorHolder(std::vector<Ch> &d, int s)  : _d(d) { _d.reserve(s); }
                            void Put(Ch c)      { if (false == (_pos==0 && c=='\"')) {_d.push_back(c); ++_pos;}  }
                            void Clear()        { _pos = 0; _d.clear(); }
                            void Flush()        {                       }
                            size_t Size() const { return _pos;          }
                        private:
                            std::vector<Ch> &_d;
                            int              _pos = 0;
                    };

                    VectorHolder vholder(cmd._buffer._buf, std::strtol(cmd._buffer._length.c_str(), nullptr, 10));
                    rapidjson::Writer<VectorHolder> writer(vholder);
                    b["Content"].Accept(writer);
                    if (vholder.Size() > 0 && cmd._buffer._buf[vholder.Size()-1] == '\"')
                        cmd._buffer._buf.pop_back();
                }
            }

            cmds.push_back(std::move(cmd));
        }

    }

    return cmds.size();
}

