/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OrcMemStreamRemoteSender.cpp
 * Author: can.gursu
 *
 * Created on 17 July 2018, 12:02
 */

#include "OrcMemStreamRemoteSender.h"

extern "C"
{
    #include "postgres.h"
}


OrcMemStreamRemoteSender::OrcMemStreamRemoteSender()
{
    Assert(true);
}

OrcMemStreamRemoteSender::OrcMemStreamRemoteSender(const OrcMemStreamRemoteSender& orig)
{
    Assert(true);
}

OrcMemStreamRemoteSender::~OrcMemStreamRemoteSender()
{
    Assert(true);
}

uint64_t OrcMemStreamRemoteSender::getLength() const
{
    Assert(true);
    return 0L;
}

uint64_t OrcMemStreamRemoteSender::getNaturalReadSize() const
{
    Assert(true);
    return 0L;
}

uint64_t OrcMemStreamRemoteSender::getNaturalWriteSize() const
{
    Assert(true);
    return 0L;
}

void OrcMemStreamRemoteSender::read(void* buf, uint64_t length, uint64_t offset)
{
    Assert(true);
}

void OrcMemStreamRemoteSender::write(const void* buf, size_t length)
{
    Assert(true);
}

const std::string& OrcMemStreamRemoteSender::getName() const
{
    Assert(true);
    return "";
}

void OrcMemStreamRemoteSender::close()
{
    Assert(true);
}
