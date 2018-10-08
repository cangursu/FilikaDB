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

Console::Console()
{
}

Console::Console(const Console& orig)
{
}

Console::~Console()
{
}


void Console::AddKeyHandler(std::vector<char> keys, const std::string &keyLabel, const std::string &desc)
{
    _keyHandlers.push_back({._keyLabel = keyLabel, ._desc = desc});
    size_t idx = _keyHandlers.size()-1;

    for (const auto &key : keys)
        _keyHandlerTable[key] = idx;
}


std::ostream &Console::DisplayMsg(const char *msg)
{
    std::cout << msg/* << std::endl*/;
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


