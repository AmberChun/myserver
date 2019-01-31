#pragma once

#include "base.h"

class EventLoop;
class Channel;
class TimeHeap : public Nocopyable
{
public:
    class TimeHeapData
    {
    public:
        TimeHeapData():time(0),cb(0){}

        bool operator<(const TimeHeapData& other) const
        {
            return time < other.time;
        }

        bool operator>(const TimeHeapData& other) const
        {
            return time > other.time;
        }

        Timestamp time;
        FuncCallback cb;
    };

    typedef std::vector<TimeHeapData> TimeHeapList;

public:
    TimeHeap(EventLoop * loop_);
    ~TimeHeap();

    //启动定时器
    void start();

public:
    //添加新定时任务
    void addTimer(int after_time,const FuncCallback& cb);
    //取出顶部任务
    void pop_back(FuncCallback& cb);

    void triggerTimeHeap();

private:
    void shiftUp(size_t pos,const TimeHeapData& data);
    void shiftDown(size_t pos,const TimeHeapData& data);

    void removeTimer(size_t pos);

    void setTimerfd();

private:
    EventLoop * loop;
    TimeHeapList timeHeapList;
    std::unique_ptr<Channel> channel;
};