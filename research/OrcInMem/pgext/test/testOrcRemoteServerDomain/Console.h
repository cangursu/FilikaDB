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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>
#include <map>
#include <atomic>


class Loop
{
public:
    Loop(const Loop &)              = delete;
    Loop(Loop &&)                   = delete;
    Loop &operator = (const Loop &) = delete;
    Loop &operator = (Loop &&)      = delete;

    Loop(auto fncPre, auto fncLoop, auto fncPost) { fncPre(); while(_exit == false) fncLoop(); fncPost();}
    virtual ~Loop() {}
    void Stop() { _exit = true; }

private:
        std::atomic<bool>   _exit  = false;
};


class Console
{
public:
    Console();
    Console(const Console& orig);
    virtual ~Console();


    std::ostream &  DisplayMsg(const char *msg);
    std::ostream &  DisplayMsg(const std::string &msg);
    std::ostream &  DisplayErrMsg(const char *msg);
    std::ostream &  DisplayErrMsg(const std::string &msg);
    void            DisplayHelp();


    bool kbhit();
    void rawmode(bool enable);



    void AddKeyHandler(std::vector<char> keys, const std::string &keyLabel, const std::string &desc);

    bool LoopListen()
    {

    }


private:

    struct KeyHandler
    {
        std::string   _keyLabel;
        std::string   _desc;
    };

    std::vector<KeyHandler> _keyHandlers;
    std::map<char, size_t>  _keyHandlerTable;

};

#endif /* CONSOLE_H */

