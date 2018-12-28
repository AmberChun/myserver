#include "Channel.h"
#include "EventLoop.h"

#include <sys/epoll.h>

const int Channel::ReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::WriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop_,int fd_)
:loop(loop_)
,fd(fd_)
,event(0)
,index(-1)
{
    
}

Channel::~Channel()
{

}

void Channel::handleEvent()
{
    if(event & EPOLLERR)
    {
       if(errorCallback) errorCallback();
    }

    if(event & (EPOLLIN | EPOLLPRI | EPOLLRDHUP ))
    {
        if(readCallback) readCallback();
    }

    if(event & EPOLLOUT)
    {
        if(writeCallback) writeCallback();
    }
}

void Channel::update()
{
    loop->updateChannel(this);
}