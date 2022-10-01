#include "EventLoop.hh"
#include "Log.hh"
#include "Channel.hh"

#include <assert.h>

using namespace net;
using namespace base;

void EventLoop::asyncHandler(EV_P_ struct ev_async*w, int revents)
{
    EventLoop* self = (EventLoop*)w->data;

    LOG_INFO("asyncHandler begin from %p", self);
    self->doPendingFunctors();
    LOG_INFO("asyncHandler end");
}

EventLoop::EventLoop() : _evLoop(NULL), _async(NULL), _tid(CurrentThread::GetThreadId())
{
    this->_evLoop = EV_DEFAULT;

    this->_async = new struct ev_async;
    _async->data = (void *)this;
    ev_async_init(this->_async, &EventLoop::asyncHandler);
    ev_async_start(this->_evLoop, this->_async);

    LOG_INFO("EventLoop object created! addr: %p", this);
}

EventLoop::~EventLoop()
{
    stop();
    ev_async_stop(this->_evLoop, this->_async);
    delete this->_async;
    ev_loop_destroy(this->_evLoop);
    LOG_INFO("EventLoop object destructed! addr: %p", this);
}

void EventLoop::loop()
{
    ev_loop(_evLoop, 0);
}

void EventLoop::stop()
{
    ev_break(_evLoop, 0);
}

void EventLoop::queueInLoopThread(const Functor& func)
{
    this->_pendingFunctors.push_back(func);
    callAsync();
}

void EventLoop::runInLoopThread(const Functor& func)
{
    if (inLoopThread()) {
        func();
    } else {
        MutexGuard guard(&_mutex);
        queueInLoopThread(func);
    }
}

bool EventLoop::inLoopThread()
{
    return CurrentThread::GetThreadId() == _tid;
}

void EventLoop::assertInLoopThread()
{
    assert(CurrentThread::GetThreadId() == _tid);
}

void EventLoop::callAsync(void)
{
    ev_async_send(this->_evLoop, this->_async);
}

void EventLoop::doPendingFunctors()
{
    MutexGuard guard(&_mutex);

    LOG_DEBUG("doPendingFunctors begin");
    for (auto it = _pendingFunctors.begin(); it != _pendingFunctors.end(); ) {
        (*it)();
        it = _pendingFunctors.erase(it);
    }
    LOG_DEBUG("doPendingFunctors end");
}

struct ev_loop* EventLoop::getEvLoop()
{
    return this->_evLoop;
}

void EventLoop::updateChannel(Channel *c)
{
    ev_io_stop(this->_evLoop, c->_watcher); // note: ev_io_set only work when watcher is inactive, so stop it first
    ev_io_set(c->_watcher, c->_fd, c->_events);
    ev_io_start(this->_evLoop, c->_watcher);
}

void EventLoop::removeChannel(Channel *c)
{
    ev_io_stop(this->_evLoop, c->_watcher);
}
