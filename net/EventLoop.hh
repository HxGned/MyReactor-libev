#ifndef __EVENT_LOOP_HH__
#define __EVENT_LOOP_HH__

#include "Callbacks.hh"
#include "Mutex.hh"

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

    void QueueInLoopThread(const Functor& func);
private:
    void callAsync();
    void doPendingFunctors();
public:
    static void asyncHandler(EV_P_ struct ev_async* w, int revents);
private:
    struct ev_loop* _loop;
    struct ev_async* _async;

    std::vector<Functor> _pendingFunctors;
    Mutex _mutex;
};

};

#endif