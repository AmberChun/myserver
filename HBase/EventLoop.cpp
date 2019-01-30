#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include "TimerQueue.h"


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
    ,timerQueue(new TimerQueue(this))
    ,wakeupChannel(new Channel(this,::eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC)))
    ,time_cache_(0)
{
    if(t_loopInThisThread)
    {
        exit(0);
    }
    else
    {
        t_loopInThisThread = this;
    }

    wakeupChannel->setReadCallback(std::bind(&EventLoop::runFuncCallback,this));
    wakeupChannel->enableReading();
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
    
    //run epoll 
    while(!quit_)
    {
        updateTimeCache();

        activeChannels.clear();
        Timestamp now = epoller->runOnce(&activeChannels);
        for (Channel* channel : activeChannels)
        {
            channel->handleEvent(now);
        }

        timerQueue->doActiveTimer();

        clearTimeCache();
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

void EventLoop::removeChannel(Channel * channel)
{
    assert(channel->getLoop() == this);
    epoller->removeChannel(channel);
}

void EventLoop::runInLoop(const FuncCallback& cb)
{
    if(isInLoopThread())
    {
        cb();
    }
    else
    {
        MutexLockGuard lock(mutex);
        funcCallbackList.push_back(cb);

        wakeup();
    }
}

void EventLoop::runFuncCallback()
{   
    handleRead();

    FuncCallbackList tempList;

    {
        MutexLockGuard lock(mutex);
        tempList.swap(funcCallbackList);
    }

    for (FuncCallback cb : tempList)
    {
        cb();
    }
}

TimerWPtr EventLoop::runAfter(const FuncCallback& cb,Timestamp when)
{
   return timerQueue->addTimer(cb,when,0);
}

TimerWPtr EventLoop::runEvery(const FuncCallback& cb,int interval)
{
   return timerQueue->addTimer(cb,1,interval);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t t = ::write(wakeupChannel->getfd(),&one,sizeof(one));
    if(t != sizeof(one))
    {
        exit(0);
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t t = read(wakeupChannel->getfd(),&one,sizeof(one));
    if( t != sizeof(one))
    {
        exit(0);
    }
}

void EventLoop::updateTimeCache()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    time_cache_ = tv.tv_sec;
}

void EventLoop::clearTimeCache()
{
    time_cache_ = 0;
}

Timestamp EventLoop::getTimeCache()
{
    return time_cache_;
}




