#include "Channel.hh"
#include "Log.hh"
#include "EventLoop.hh"

#include "Common.hh"

#include <poll.h>

using namespace base;
using namespace net;
using namespace std;

// event attribute for libev
const int Channel::kReadEvent = EV_READ;
const int Channel::kWriteEvent = EV_WRITE;
const int Channel::kNoneEvent = EV_NONE;

Channel::Channel(EventLoop* loop, int fd) : _loop(loop), _fd(fd), _index(kInitialIndex), _events(0), _rEvents(0), _isWriteEnabled(false), \
    _watcher(NULL)
{
    LOG_INFO("Channel construct");

    // get ev_io watcher ready, but do not regist any events
    _watcher = new struct ev_io;
    ev_io_init(this->_watcher, &(Channel::ChannelWatcherCallback), this->_fd, EV_NONE);
    this->_watcher->data = (void*)this;
    ev_io_start(this->_loop->getEvLoop(), this->_watcher);
}

Channel::~Channel()
{
    LOG_INFO("Channel destruct");

    // deallocate watcher space
    ev_io_stop(this->_loop->getEvLoop(), this->_watcher);
    delete this->_watcher;
}

bool Channel::IsWriteEnabled()
{
    return _isWriteEnabled;
}

void Channel::SetIndex(int index)
{
    this->_index = index;
}

int Channel::GetIndex()
{
    return _index;
}

// 返回channel对应的fd
int Channel::GetFd()
{
    return _fd;
}
// 返回channel对应的loop
EventLoop* Channel::GetEventLoop()
{
    return _loop;
}

// events && revents
int Channel::GetEvents()
{
    return _events;
}
int Channel::GetrEvents()
{
    return _rEvents;
}

// epoll_wait返回后调用
void Channel::SetrEvents(int events)
{
    this->_rEvents = events;
}

// 设置Channel对应事件的回调
void Channel::SetReadCallback(const ReadCallback& cb)
{
    _readCallback = cb;
}
void Channel::SetWriteCallback(const WriteCallback& cb)
{
    _writeCallback = cb;
}
void Channel::SetCloseCallback(const CloseCallback& cb)
{
    _closeCallback = cb;
}
void Channel::SetErrorCallback(const ErrorCallback& cb)
{
    _errorCallback = cb;
}

// 开启/关闭读写
void Channel::EnableRead()
{
    this->_events |= kReadEvent;
    Update();
}
void Channel::DisableRead()
{
    this->_events &= ~kReadEvent;
    Update();
}
void Channel::EnableWrite()
{
    this->_events |= kWriteEvent;
    Update();
    _isWriteEnabled = true;
}
void Channel::DisableWrite()
{
    this->_events &= ~kWriteEvent;
    Update();
    _isWriteEnabled = false;
}
void Channel::DisableAll()
{
    this->_events = kNoneEvent;
    // this->_index = kDeleteIndex; // done this in EpollPoller
    Update();
}

void Channel::HandleEvent(Timestamp timestamp)
{
    LOG_DEBUG("HandleEvent begin");
    LOG_DEBUG("timestamp: [%s], fd: [%d]", timestamp.ConvertToString().c_str(), this->_fd);

    // libev do not support this
    // 连接关闭
    /* if (_rEvents & EPOLLHUP && !(_rEvents & EPOLLIN)) {
        LOG_INFO("close event");
        if (_closeCallback) {
            _closeCallback();
        }
    } */

    // 错误
    if (_rEvents & (EV_ERROR)) {
        LOG_INFO("error event");
        if (_errorCallback) {
            _errorCallback();
        }
    }
    // 读取
    if (_rEvents & (EV_READ)) {
        LOG_INFO("read event");
        if (_readCallback) {
            _readCallback(timestamp);
        }
    }
    // 可写
    if (_rEvents & (EV_WRITE)) {
        LOG_DEBUG("write event");
        if (_writeCallback) {
            _writeCallback();
        }
    }

    LOG_DEBUG("HandleEvent end");
}

void Channel::Update()
{
    // _loop->UpdateChannel(this);
    ev_io_set(this->_watcher, this->_fd, _events);
}

void Channel::Remove()
{
    // _loop->RemoveChannel(this);
    ev_io_stop(this->_loop->getEvLoop(), this->_watcher);
}

string Channel::Events2String(int events)
{
    string evStr = "";

    if (events & kReadEvent) {
        evStr += "|EV_READ";
    }
    if (events & kWriteEvent) {
        evStr += "|EV_WRITE";
    }
    evStr += "|";

    return evStr;
}

const char* Channel::Index2String(int index)
{
    switch (index) {
        case kInitialIndex:
            return "Initial";
        case kAddedIndex:
            return "Added";
        case kDeleteIndex:
            return "Deleted";
        default:
            return "Unknown";
    }
    return "Unknown";
}

void Channel::ChannelWatcherCallback(EV_P_ ev_io* w, int revents)
{
    LOG_DEBUG("ChannelWatcherCallback begin!");
    Timestamp now;
    Channel* self = (Channel *)(w->data);
    self->_rEvents = revents;

    // call member function to handle Event
    self->HandleEvent(now);
    LOG_DEBUG("ChannelWatcherCallback end!");
}
