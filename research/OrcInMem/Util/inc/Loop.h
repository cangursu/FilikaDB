/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Loop.h
 * Author: can.gursu
 *
 * Created on 15 October 2018, 09:07
 */

#ifndef __LOOP_H__
#define __LOOP_H__

#include "TimeUtils.h"

#include <atomic>
#include <functional>
#include <iostream>

class Loop
{
public:
    Loop(const Loop &)              = delete;
    Loop(Loop &&)                   = delete;

    Loop &operator = (const Loop &) = delete;
    Loop &operator = (Loop &&)      = delete;

    virtual ~Loop()                             { Stop();               }
    Loop(const std::string &name) : _name(name) {                       }
    Loop(auto fncLoop, auto fncPre = _emptyFctrBool, auto fncPost = _emptyFctrVoid)    { Start(fncLoop, fncPre, fncPost);    }

    void PreFctor  (std::function<bool()> fctr) { _fctrPreLoop  = fctr; }
    void PostFctor (std::function<void()> fctr) { _fctrPostLoop = fctr; }
    void Fctor     (std::function<bool()> fctr) { _fctrLoop     = fctr; }

    std::string Name() const                    { return _name;         }
    void Name(std::string &name)                { _name = name;         }

    bool Stat()                                 { return _exit;         }
    void Stop()                                 { _exit = true;         }
    void Start()
    {
        if (true == _fctrPreLoop())
        {
            while((false == _exit) && (true == _fctrLoop()))
                nsleep(100);
        }
        _fctrPostLoop();
    }

    void Start(auto fncLoop, std::function<bool()>  fncPre = Loop::_emptyFctrBool,  std::function<void()>  fncPost = _emptyFctrVoid)
    {
        PreFctor(fncPre);
        PostFctor(fncPost);
        Fctor(fncLoop);

        Start();
    }


private:

    static std::function<bool()>    _emptyFctrBool;
    static std::function<void()>    _emptyFctrVoid;

    std::atomic<bool>               _exit       = false;
    std::string                     _name;


    std::function<bool()>           _fctrPreLoop    = _emptyFctrBool;
    std::function<void()>           _fctrPostLoop   = _emptyFctrVoid;
    std::function<bool()>           _fctrLoop       = _emptyFctrBool;
};

#endif /* LOOP_H */

