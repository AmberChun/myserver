#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"

#include <stdlib.h> //exit
#include <assert.h> //assert
#include <stdio.h> //printf
#include <unistd.h> //syscall
#include <sys/syscall.h> //syscall‘s param


__thread EventLoop* t_loopInThisThread = 0;

static pid_t gettid()
{
	return static_cast<pid_t>(::syscall(SYS_gettid));
}

EventLoop::EventLoop()
    :isLooping(false)
    ,quit_(false)
    ,threadId(gettid())
    ,epoller(new Epoller(this))
{
    if(t_loopInThisThread)
    {
        exit(0);
    }
    else
    {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    t_loopInThisThread = NULL;
}

bool EventLoop::isInLoopThread() 
{ 
    return threadId == gettid();
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::loop()
{
    assert(!isLooping);
    assert(!isInLoopThread());
    isLooping = true;
    quit_ = false;
    
    //run poll or epoll
    while(!quit_)
    {
        activeChannels.clear();
        epoller->runOnce(&activeChannels);
        for (Channel* channel : activeChannels)
        {
            channel->handleEvent();
        }
    }

    

    isLooping = false;
}

void EventLoop::quit()
{
    quit_ = true;
}

void EventLoop::updateChannel(Channel * channel)
{
    assert(channel->getLoop() == this);
    epoller->updateChannel(channel);
}


