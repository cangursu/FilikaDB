/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Console.h
 * Author: can.gursu
 *
 * Created on 24 September 2018, 15:16
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "GeneralUtils.h"
#include "Loop.h"

#include <string>
#include <vector>
#include <map>

#include <iostream>
#include <sstream>
#include <termios.h>
#include <sys/ioctl.h>
#include <iomanip>
#include <unistd.h>




class Console : public Loop
{
public:
    struct KeyHandler
    {
        std::string         _keyLabel;
        std::string         _desc;
        std::int32_t       _id        = -1;
    };


    Console()                       { }
    Console(const Console& orig)    { }
    virtual ~Console()              { }

    virtual void EventFired(const Console::KeyHandler &kh) = 0;

    void LoopStart();
    void LoopStop() {Loop::Stop();}

    std::ostream &  DisplayMsg(const char *msg);
    std::ostream &  DisplayMsg(const std::string &msg);
    std::ostream &  DisplayErrMsg(const char *msg);
    std::ostream &  DisplayErrMsg(const std::string &msg);
    void            DisplayHelp();


    bool kbhit();
    void rawmode(bool enable);



    void KeyHandlerAdd(std::vector<char> keys, int id, const std::string &keyLabel, const std::string &desc);
    bool KeyHandlerGet(char key, KeyHandler &kh) const;

private:

    std::vector<KeyHandler> _keyHandlers;
    std::map<char, size_t>  _keyHandlerTable;

};



#endif /* __CONSOLE_H__ */

