/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Console.cpp
 * Author: can.gursu
 *
 * Created on 24 September 2018, 15:16
 */

#include "Console.h"

#include <iostream>
#include <sstream>
#include <termios.h>
#include <sys/ioctl.h>
#include <iomanip>


void Console::KeyHandlerAdd(std::vector<char> keys, int id, const std::string &keyLabel, const std::string &desc)
{
    _keyHandlers.push_back({._keyLabel = keyLabel, ._desc = desc, ._id = id});
    size_t idx = _keyHandlers.size()-1;

    for (const auto &key : keys)
        _keyHandlerTable[key] = idx;
}

bool Console::KeyHandlerGet(char key, KeyHandler &kh) const
{
    const auto &it = _keyHandlerTable.find(key);
    if (it != _keyHandlerTable.cend())
    {
        kh = _keyHandlers[it->second];
        return true;
    }
    return false;
}


std::ostream &Console::DisplayMsg(const char *msg)
{
    std::cout << msg << std::endl;
    std::cout.flush();
    return std::cout;
}


std::ostream &Console::DisplayMsg(const std::string &msg)
{
    return DisplayMsg(msg.c_str());
}


std::ostream &Console::DisplayErrMsg(const char *msg)
{
    std::cerr << msg/* << std::endl*/;
    return std::cerr;
}


std::ostream &Console::DisplayErrMsg(const std::string &msg)
{
    return DisplayErrMsg(msg.c_str());
}


void Console::DisplayHelp()
{
    std::stringstream ss;

    ss << std::endl;
    ss << "Keybord Handlers : \n";
    for (const auto &kh : _keyHandlers)
        ss << std::setw(6) << kh._keyLabel << "  --> " << kh._desc << std::endl;
    ss << std::endl;

    std::cout << ss.str();
}


bool Console::kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}


void Console::rawmode(bool enable)
{
    termios term;
    tcgetattr(0, &term);
    if (enable)
        term.c_lflag &= ~(ICANON | ECHO);
    else
        term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}


void Console::LoopStart()
{
    Loop::Start (
            [this]()->bool
                    {
                        char ch;
                        if (kbhit())
                        {
                            read(STDIN_FILENO, &ch, 1);

                            KeyHandler kh;
                            if (true == KeyHandlerGet(ch, kh))
                            {
                                EventFired(kh);
                            }
                        }
                        msleep(500);
                        return true;
                    } ,
            [this]()->bool { rawmode(true); return true; } ,
            [this]()->void { rawmode(false); }
        );
}


