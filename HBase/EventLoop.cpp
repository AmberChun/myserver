#include "EventLoop.h"
#include <stdlib.h> //exit
#include <assert.h> //assert
#include <stdio.h> //printf
__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop()
    :isLooping(false)
    ,threadId(gettid())
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

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::loop()
{
    assert(!isLooping);
    assert(!isInLoopThread());
    isLooping = true;
    
    //run poll or epoll
    printf("loop sucess! %d \n",threadId); 
    isLooping = false;
}




