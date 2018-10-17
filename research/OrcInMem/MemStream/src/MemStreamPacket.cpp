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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "libbase64.h"

#include <ctime>
#include <vector>




MemStreamPacket::MemStreamPacket()
{
}


MemStreamPacket::~MemStreamPacket()
{
}

std::uint64_t MemStreamPacket::CreateRefID()
{
    static std::uint64_t uuid = 1;
    return uuid++;
}



void MemStreamPacket::JsonDataB64(rapidjson::Value &cmdItem,
                                        rapidjson::Document::AllocatorType &a,
                                        std::size_t length,
                                        std::size_t offset /*= 0*/,
                                        const void *data /*= nullptr*/)
{
    if (data)
    {
        size_t outlen = (4.0*length/3.0)+2;
        char out[outlen] = "";
        base64_encode((char*)data, length, out, &outlen, BASE64_FORCE_AVX2);

        rapidjson::Value buffItem;
        buffItem.SetObject();
        buffItem.AddMember("Lenght" , rapidjson::Value (std::to_string(outlen).c_str() , a), a);
        buffItem.AddMember("Enc"    , "Base64"                         , a);
        if (offset>0) buffItem.AddMember("Offset" , offset, a);
        buffItem.AddMember("Content", rapidjson::Value (out, outlen, a), a);

        cmdItem.AddMember("Buffer", buffItem, a);
    }
}

void MemStreamPacket::JsonBase(rapidjson::Value &cmdItem,
                                     rapidjson::Document::AllocatorType &a,
                                     CmdID cmd,
                                     const char *source)
{
    std::time_t tmeSmp = std::time(nullptr);

    cmdItem.AddMember("Sender",    "SenderXXX"                                                      , a);
    cmdItem.AddMember("Timestamp", rapidjson::Value(std::to_string(tmeSmp).c_str()            , a)  , a);
    cmdItem.AddMember("Source",    rapidjson::Value(source                                    , a)  , a);
    cmdItem.AddMember("CmdID",     rapidjson::Value(_strCmd[cmd].c_str()                      , a)  , a);
    cmdItem.AddMember("RefID",     rapidjson::Value(std::to_string(CreateRefID()).c_str()     , a)  , a);
    cmdItem.AddMember("Status",    rapidjson::Value(_strCStat[CmdStatus::CST_REQUEST].c_str() , a)  , a);

}

std::uint32_t MemStreamPacket::CreatePacketWrite(const char *source, const void *data, std::size_t len)
{
    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Document::AllocatorType &a = doc.GetAllocator();
    rapidjson::Value cmdItem;
    cmdItem.SetObject();

    JsonBase(cmdItem, a, CmdID::CMD_WRITE, source);
    JsonDataB64(cmdItem, a, len, 0, data);


    rapidjson::Value cmdArray(rapidjson::kArrayType);
    cmdArray.PushBack(cmdItem, a);
    doc.AddMember("MYSFRIF", cmdArray, a);

    rapidjson::StringBuffer sbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sbuf);
    doc.Accept(writer);

    Reset();
    return StreamPacket::Create(sbuf.GetString(), sbuf.GetSize());
}

int  MemStreamPacket::DecodePacket (const StreamPacket &packet, std::vector<Cmd> &cmds)
{
    StreamPacket::byte_t payload[512] = "";
    if (1 < packet.Payload(payload, 512))
    {
        rapidjson::Document doc;
        doc.Parse((char*)payload);
        if (false == doc.IsObject() || false == doc.HasMember("MYSFRIF"))
            return 0;
        if(false == doc["MYSFRIF"].IsArray())
            return 0;

        cmds.empty();

        for (auto& v : doc["MYSFRIF"].GetArray())
        {
            // Parameter Validation
            if (false == v.HasMember("CmdID"))     break;
            if (false == v.HasMember("Source"))    break;
            if (false == v.HasMember("Sender"))    break;
            if (false == v.HasMember("Timestamp")) break;
            if (false == v.HasMember("RefID"))     break;
            if (false == v.HasMember("Status"))    break;

            Cmd cmd;
            cmd._cmdidStr = v["CmdID"].GetString();;
            cmd._source   = v["Source"].GetString();
            cmd._sender   = v["Sender"].GetString();
            cmd._ts       = v["Timestamp"].GetString();
            cmd._refid    = v["RefID"].GetString();
            cmd._result   = v["Status"].GetString();

            for(const auto &s : _strCmd)
                if (s.second == cmd._cmdidStr) cmd._cmdid = s.first;

            if (v.HasMember("Message")) cmd._result = v["Message"].GetString();
            if (v.HasMember("Buffer"))
            {
                auto &b = v["Buffer"];
                if (b.HasMember("Lenght")) cmd._buffer._length = b["Lenght"].GetString();
                if (b.HasMember("Offset")) cmd._buffer._offset = b["Offset"].GetString();
                if (b.HasMember("Enc"))    cmd._buffer._enc    = b["Enc"].GetString();
                if (b.HasMember("Content"))
                {
                    class VectorHolder
                    {
                        public:
                            typedef char Ch;
                            VectorHolder(std::vector<Ch> &d, int s)  : _d(d) { _d.reserve(s); }
                            void Put(Ch c)      { if (false == (_pos==0 && c=='\"'))  _d.push_back(c);  }
                            void Clear()        { _pos = 0; _d.clear(); }
                            void Flush()        { return;               }
                            size_t Size() const { return _pos;          }//_d.size(); }
                        private:
                            std::vector<Ch> &_d;
                            int              _pos = 0;
                    };

                    VectorHolder vholder(cmd._buffer._buf, std::stol(cmd._buffer._length));
                    rapidjson::Writer<VectorHolder> writer(vholder);
                    b["Content"].Accept(writer);
                    if (vholder.Size() > 0 && cmd._buffer._buf[vholder.Size()] == '\"')
                        cmd._buffer._buf.pop_back();
                }
            }
            cmds.push_back(std::move(cmd));
        }
    }

    return cmds.size();
}

