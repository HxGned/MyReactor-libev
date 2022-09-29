#include "EventLoop.hh"
#include "Log.hh"

using namespace net;

// TODO: why this can't be a static member function
// if we put it here, what is its scope
static void asyncHandler(EV_P_ struct ev_async*w, int revents)
{
    EventLoop* self = (EventLoop*)w->data;

    LOG_INFO("asyncHandler begin from %p", self);
    LOG_INFO("asyncHandler end");
}

EventLoop::EventLoop() : _loop(NULL), _async(NULL)
{
    this->_loop = EV_DEFAULT;

    this->_async = new struct ev_async;
    _async->data = (void *)this;
    ev_async_init(this->_async, &asyncHandler);
    ev_async_start(this->_loop, this->_async);

    LOG_INFO("EventLoop object created! addr: %p", this);
}

EventLoop::~EventLoop()
{
    stop();
    ev_async_stop(this->_loop, this->_async);
    delete this->_async;
    ev_loop_destroy(this->_loop);
    LOG_INFO("EventLoop object destructed! addr: %p", this);
}

void EventLoop::loop()
{
    ev_loop(_loop, 0);
}

void EventLoop::stop()
{
    ev_break(_loop, 0);
}

void EventLoop::callAsync(void)
{
    ev_async_send(this->_loop, this->_async);
}
