/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   flowlog_utilities.h
 * Author: can.gursu
 *
 * Created on 25 April 2018, 10:06
 */

#ifndef __FLOWLOG_UTILITIES_H__
#define __FLOWLOG_UTILITIES_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <postgres.h>
#include <executor/tuptable.h>

#ifdef __cplusplus
}
#endif



#include "string"


std::string text_enum(CmdType val);
std::string text_enum(NodeTag val);
std::string text_enum(Oid val);
std::string text_enum(TupleTableSlot *slot);


#endif /* __FLOWLOG_UTILITIES_H__ */

