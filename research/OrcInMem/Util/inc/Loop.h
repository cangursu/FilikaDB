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

#include <atomic>
#include <functional>

class Loop
{
public:
    Loop(const Loop &)              = delete;
    Loop(Loop &&)                   = delete;
    Loop &operator = (const Loop &) = delete;
    Loop &operator = (Loop &&)      = delete;

    virtual ~Loop()                 { Stop(); }
    Loop()                          {         }
    Loop(auto fncLoop, auto fncPre = _emptyFctr, auto fncPost = _emptyFctr)    { Start(fncLoop, fncPre, fncPost);  }

    void PreFctor(auto fctr)        { _fctrPreLoop  = fctr; }
    void PostFctor(auto fctr)       { _fctrPostLoop = fctr; }
    void Fctor(auto fctr)           { _fctrLoop     = fctr; }

    void Stop()                     { _exit         = true; }
    void Start()
    {
    _fctrPreLoop();
    while(_exit == false) _fctrLoop();
    _fctrPostLoop();
}

    void Start(auto fncLoop, auto fncPre = _emptyFctr, auto fncPost = _emptyFctr)
    {
    PreFctor(fncPre);
    PostFctor(fncPost);
    Fctor(fncLoop);

    Start();
}


private:

    static std::function<void()>    _emptyFctr;
    std::atomic<bool>               _exit  = false;

    std::function<void()>           _fctrPreLoop    = _emptyFctr;
    std::function<void()>           _fctrPostLoop   = _emptyFctr;
    std::function<void()>           _fctrLoop       = _emptyFctr;
};

#endif /* LOOP_H */

