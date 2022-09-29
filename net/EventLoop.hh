#ifndef __EVENT_LOOP_HH__
#define __EVENT_LOOP_HH__

#include "Callbacks.hh"

#include <vector>
#include <ev.h>

namespace net {

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void stop();
    void callAsync();
public:

private:
    struct ev_loop* _loop;
    struct ev_async* _async;

    std::vector<Functor> _pendingFunctors;
};

};

#endif