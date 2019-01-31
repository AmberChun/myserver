#include "TimeHeap.h"
#include "EventLoop.h"
#include "Channel.h"
#include <sys/timerfd.h>

TimeHeap::TimeHeap(EventLoop * loop_)
    :loop(loop_)
    ,channel(new Channel(loop_,::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC)))
{

}

TimeHeap::~TimeHeap()
{
    
}

void TimeHeap::start()
{
    channel->setReadCallback(std::bind(&TimeHeap::triggerTimeHeap,this));
    channel->enableReading();

    setTimerfd();
}

void TimeHeap::addTimer(int after_time,const FuncCallback& cb)
{
    TimeHeapData data;
    data.time = after_time + loop->Now();
    data.cb = cb;

    timeHeapList.push_back(data);
    shiftUp(timeHeapList.size() - 1,data);

    setTimerfd();
}

void TimeHeap::pop_back(FuncCallback& cb)
{
    if(timeHeapList.empty())
    {   
        return;
    }

    cb = timeHeapList[0].cb;
    removeTimer(0);
}

void TimeHeap::shiftUp(size_t pos,const TimeHeapData& data)
{
    size_t parent_pos = (pos - 1)/2;

    while(data < timeHeapList[parent_pos])
    {
        timeHeapList[pos] = timeHeapList[parent_pos];
        pos = parent_pos;
        parent_pos = (pos - 1)/2;
    }

    timeHeapList[pos] = data;
}

void TimeHeap::shiftDown(size_t pos,const TimeHeapData& data)
{
    size_t child_pos = (pos + 1) * 2;
    while(child_pos <= timeHeapList.size())
    {
        child_pos -= child_pos == timeHeapList.size() || timeHeapList[child_pos] > timeHeapList[child_pos - 1];
        if(!(data > timeHeapList[child_pos]))
        {
            break;
        }

        pos = child_pos;
        child_pos = (pos + 1) * 2;
    }

    timeHeapList[pos] = data;
}

void TimeHeap::removeTimer(size_t pos)
{
    if(timeHeapList.empty() || pos >= timeHeapList.size())
    {
        return;
    }

    timeHeapList[pos] = timeHeapList[timeHeapList.size() - 1];
    timeHeapList.pop_back();
    shiftDown(pos,timeHeapList[pos]);
}

void TimeHeap::triggerTimeHeap()
{
    uint64_t val = 0;
    ssize_t s = read(channel->getfd(),&val,sizeof(val));
    if(s <= 0)
    {
        return;
    }

    if(timeHeapList.empty())
    {
        return;
    }

    timeHeapList[0].cb();
    removeTimer(0);

    setTimerfd();
}

void TimeHeap::setTimerfd()
{
    struct itimerspec howlong;
    ::timerfd_gettime(channel->getfd(),&howlong);

    if(howlong.it_value.tv_sec > 0 || howlong.it_value.tv_nsec > 0)
    {
        return;
    }

    if(timeHeapList.empty())
    {
        return;
    }

    memset(&howlong,0,sizeof(howlong));
    howlong.it_value.tv_sec = timeHeapList[0].time > loop->Now() ? timeHeapList[0].time - loop->Now():1;
    howlong.it_interval.tv_sec = 0;
    
    ::timerfd_settime(channel->getfd(),0,&howlong,NULL);
}