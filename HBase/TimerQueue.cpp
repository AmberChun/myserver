#include "TimerQueue.h"

#include <sys/timerfd.h>

Timer::Timer(TimerCallback cb_,Timestamp when,Timestamp interval_,int countMax_)
:cb(cb_)
,expiration(when)
,interval(interval_)
,repeat(interval > 0.0)
,count(0)
,countMax(countMax_)
{

}

Timer::~Timer()
{
    
}

void Timer::runCallback()
{
    if(countMax > 0 && count < countMax)
    {
        
    }

    cb();
}

Timestamp Timer::restart(Timestamp now)
{
    //这里没判断为了让外部调用repeated(),为了防止判断两次
    expiration = now + interval;

    return expiration;
}

bool Timer::repeated()
{
    return repeat && (countMax > 0 ? count < countMax : true );
}

TimerQueue::TimerQueue(EventLoop *loop_)
:timerChannel(loop_,::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC))
{
    struct itimerspec howlong;
    memset(&howlong,0,sizeof(howlong));
    howlong.it_value.tv_sec = 1;
    howlong.it_interval.tv_sec = 1;
    ::timerfd_settime(timerChannel.getfd(),0,&howlong,NULL);
 
    timerChannel.setReadCallback(std::bind(&TimerQueue::update,this));
    timerChannel.enableReading();
}

TimerQueue::~TimerQueue()
{
    timerChannel.disableAll();
}

void TimerQueue::addTimer(const TimerCallback& cb,Timestamp when,Timestamp interval,int countMax_)
{
    Timestamp now = Now();
    Timer timer(cb,now + when,interval,countMax_);

    {
        MutexLockGuard lock(mutex);
        timerMap.insert(std::make_pair(now + when,timer));
    }
}

void TimerQueue::update()
{
    MutexLockGuard lock(mutex);
    recycleTimer();
    uint64_t val = 0;
    val = read(timerChannel.getfd(),&val,sizeof(val));
    Timestamp now = Now();
    TimerMultiMap::iterator it = timerMap.lower_bound(now);
    if(it == timerMap.begin())
    {
        return;
    }

    std::copy(timerMap.begin(),it,back_inserter(activeTimerList));
    timerMap.erase(timerMap.begin(),it);
}

void TimerQueue::doActiveTimer()
{
    Timestamp now = Now();
    for(ActiveTimerList::iterator it = activeTimerList.begin(); it != activeTimerList.end();++it)
    {
        it->second.runCallback();

        if(it->second.repeated())
        {
            Timestamp ts = it->second.restart(now);
            it->first = ts;
            recycleTimerList.push_back(*it);
        }
    }


    activeTimerList.clear();
}

void TimerQueue::recycleTimer()
{
    for(RecycleTimerList::iterator it = recycleTimerList.begin(); it != recycleTimerList.end();++it)
    {
        timerMap.insert(*it);
    }

    recycleTimerList.clear();
}

Timestamp TimerQueue::Now()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec;
}


