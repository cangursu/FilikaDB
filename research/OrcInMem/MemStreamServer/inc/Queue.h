//
// Copyright (c) 2013 Juan Palacios juan.palacios.puyana@gmail.com
// Subject to the BSD 2-Clause License
// - see < http://opensource.org/licenses/BSD-2-Clause>
//

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class Queue
{
    public:
/*
        T&& pop()
        {
            std::unique_lock<std::mutex> mlock(_mutex);
            while (_queue.empty())
                _cond.wait(mlock);
            auto val = _queue.front();
            _queue.pop();
            return std::move(val);
        }
*/
        bool pop(T& item)
        {
            std::unique_lock<std::mutex> mlock(_mutex);
            if (std::cv_status::no_timeout  == _cond.wait_for(mlock, std::chrono::seconds(1)))
            {
                item = std::move(_queue.front());
                _queue.pop();
                return true;
            }

            return false;
        }

        void push(const T& item)
        {
            std::unique_lock<std::mutex> mlock(_mutex);
            _queue.push(item);
            mlock.unlock();
            _cond.notify_one();
        }

        Queue()                          = default;
        Queue(const Queue&)              = delete;
        Queue(Queue&&)                   = delete;
        Queue& operator=(const Queue &)  = delete;
        Queue& operator=(Queue &&)       = delete;

    private:
        std::queue<T>           _queue;
        std::mutex              _mutex;
        std::condition_variable _cond;
};

#endif  //__QUEUE_H__
