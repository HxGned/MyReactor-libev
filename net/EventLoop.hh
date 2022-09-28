#ifndef __EVENT_LOOP_HH__
#define __EVENT_LOOP_HH__

#include <ev.h>

namespace net {

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void stop();
private:
    struct ev_loop* _loop;
};

};

#endif