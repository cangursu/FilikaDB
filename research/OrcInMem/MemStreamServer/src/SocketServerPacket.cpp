/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SocketServerPacket.cpp
 * Author: can.gursu
 *
 * Created on 19 July 2018, 15:22
 */

#include "SocketServerPacket.h"
#include "LoggerGlobal.h"
#include "SocketUtils.h"

#include <cstring>
#include <iostream>

#include <unistd.h>
#include <algorithm>



/*
SourceChannel::SourceChannel(const char *sourceChannelName)
        : SocketDomain(sourceChannelName, "SourceChannel")
{
}
*/

/*
ParseResult SourceChannelClient::recvScanMID(byte_t by, msize_t posBuff, msize_t posPacket)
{
//    LOG_LINE_GLOBAL("SServerClient", "---> by:", by, "(", std::hex, by, std::dec, ")", " posBuff:", posBuff, " posPacket:", posPacket);

    ParseResult res = ParseResult::ERROR;

    if ((posPacket >= g_posMID) && posPacket < g_posMID + g_lenMID)
    {
        if (posPacket == 0)
            _posBuffBeginPack = posBuff;
        if (by == StreamPacket::s_mid[posPacket])
            res = (posPacket == (g_posMID + g_lenMID - 1)) ? ParseResult::NEXT : ParseResult::CONTINUE;
    }

//    LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
    return res;
}
*/

/*
ParseResult SourceChannelClient::recvParseDataLength(byte_t by, msize_t posBuff, msize_t posPacket)
{
//    LOG_LINE_GLOBAL("SServerClient", "---> posBuff:", posBuff, " posPacket:", posPacket);

    ParseResult res = ParseResult::ERROR;
    if ((posPacket >= g_posPLen) && (posPacket < g_posPLen + g_lenPLen))
    {
        _lenghtBuff[_lenghtBuffPos++] = by;
        if (posPacket == g_posPLen + g_lenPLen - 1)
        {
            _lenPayload    = 0L;
            _lenghtBuffPos = 0L;

            memcpy(&_lenPayload, _lenghtBuff, g_lenPLen);
            //LOG_LINE_GLOBAL("SServerClient", "_lenPayload:", _lenPayload);
            res = (_lenPayload > g_lenMaxBuffer) ? ParseResult::ERROR : ParseResult::NEXT;
        }
        else
        {
            res = ParseResult::CONTINUE;
        }
    }
    else
    {
        _lenghtBuffPos = 0;
    }

//    LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
    return res;
}
*/

/*
ParseResult SourceChannelClient::recvRest (byte_t , msize_t posBuff, msize_t posPacket)
{
//    LOG_LINE_GLOBAL("SServerClient", "---> posBuff:", posBuff, " posPacket:", posPacket, " g_posPayload:", g_posPayload, " _lenPayload:", _lenPayload);

    ParseResult res = ParseResult::ERROR;
    if ((posPacket >= g_posPayload) && (posPacket < g_posPayload + _lenPayload + g_lenCRC - 1))
        res = ParseResult::CONTINUE;
    else if (posPacket == g_posPayload + _lenPayload + g_lenCRC  - 1)
        res = ParseResult::FINISH;

//    LOG_LINE_GLOBAL("SServerClient", "---< res = ", ParseResultText(res));
    return res;
}
*/

