#include "Channel.h"
#include "EventLoop.h"

#include <sys/epoll.h>

const int Channel::ReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::WriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop_,int fd_)
:loop(loop_)
,fd(fd_)
,event(0)
,revent(0)
,index(-1)
{
    
}

Channel::~Channel()
{
    ::close(fd);
}

void Channel::handleEvent()
{
    if(revent & EPOLLERR)
    {
       if(errorCallback) errorCallback();
    }

    if((revent & EPOLLHUP) && !( revent & EPOLLIN))
    {
        if(closeCallback) closeCallback();
    }

    if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP ))
    {
        if(readCallback) readCallback();
    }

    if(revent & EPOLLOUT)
    {
        if(writeCallback) writeCallback();
    }
}

void Channel::update()
{
    loop->updateChannel(this);
}

void Channel::disable()
{
    loop->removeChannel(this);
}