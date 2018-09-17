/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.h
 * Author: can.gursu
 *
 * Created on 10 September 2018, 09:59
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "SocketServer.h"
#include "SocketClient.h"
#include "MemStream.h"

int msleep(long miliseconds);
int nsleep(long nanoseconds);

std::string MemStream2String(MemStream<std::uint8_t> &stream);


extern const char *g_slog;
extern const char *g_ssrv;


#endif /* __MAIN_H__ */

