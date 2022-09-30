#ifndef __EVENT_LOOP_HH__
#define __EVENT_LOOP_HH__

#include "Callbacks.hh"
#include "Mutex.hh"
#include "CurrentThread.hh"

#include <vector>
#include <ev.h>

using base::Mutex;

namespace net {

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void stop();

    void assertInLoopThread();
    bool inLoopThread();
    void queueInLoopThread(const Functor& func);
    void runInLoopThread(const Functor& func);
    struct ev_loop* getEvLoop();
private:
    void callAsync();
    void doPendingFunctors();
public:
    static void asyncHandler(EV_P_ struct ev_async* w, int revents);
private:
    struct ev_loop* _evLoop;
    struct ev_async* _async;
    pid_t _tid;

    std::vector<Functor> _pendingFunctors;
    Mutex _mutex;
};

};

#endif