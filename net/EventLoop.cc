#include "EventLoop.hh"
#include "Log.hh"

using namespace net;

EventLoop::EventLoop() : _loop(NULL)
{
    this->_loop = EV_DEFAULT;
}

EventLoop::~EventLoop()
{

}

void EventLoop::loop()
{
    ev_loop(_loop, 0);
}

void EventLoop::stop()
{
    ev_break(_loop, 1);
}