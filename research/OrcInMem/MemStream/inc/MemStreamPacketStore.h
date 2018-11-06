/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcStreamRemotePacket.h
 * Author: can.gursu
 *
 * Created on 27 September 2018, 11:07
 */

#ifndef __MEM_STREAM_PACKET_MAP_H__
#define __MEM_STREAM_PACKET_MAP_H__

#include "MemStream.h"

#include <unordered_map>


class MemStreamPacketMap
{
    public :

        void Write(const char *source, const void* buf, size_t length)
        {
            _sourceMap[source].Write(buf, length);
        }

        std::uint64_t Read(const char *source, void* buf, std::uint64_t length, std::uint64_t offset) const
        {
            if(_sourceMap.find(source) == _sourceMap.end())
                return (std::uint64_t)-1;
            return _sourceMap.at(source).Read(buf, length, offset);
        }

        size_t Delete(const char *source)
        {
            if(_sourceMap.find(source) == _sourceMap.end())
                return (std::uint64_t)-1;
            return _sourceMap.erase(source);
        }

        std::uint64_t   Size    (const char *source) const
        {
            if(_sourceMap.find(source) == _sourceMap.end())
                return (std::uint64_t)-1;
            return _sourceMap.at(source).Size();
        }

        std::uint64_t   Len     (const char *source) const
        {
            if(_sourceMap.find(source) == _sourceMap.end())
                return (std::uint64_t)-1;
            return _sourceMap.at(source).Len();
        }

        std::uint64_t   Cnt     (const char *source) const
        {
            if(_sourceMap.find(source) == _sourceMap.end())
                return (std::uint64_t)-1;
            return _sourceMap.at(source).Cnt();
        }

    private :
        std::unordered_map<std::string, MemStream<std::uint8_t> > _sourceMap;
};


#endif /* __MEM_STREAM_PACKET_MAP_H__ */

