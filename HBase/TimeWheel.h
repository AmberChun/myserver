#pragma once

#include "base.h"

//未加锁，只支持单线程，不可跨线程调用addTimer

class EventLoop;
class Channel;
class TimeWheel: public Nocopyable
{
public:
    struct TimeData
    {
        int time;
        FuncCallback cb;
    };
public:
    //min_time_interval 一格格子多少时间 （秒）
    //grid_num 一个时间轮有多少格子
    //circle_num 共有几个时间轮 决定了定时器最大能计算多长时间 ：
    //      max_time = min_time_interval * (wheel_num^circle_num)
    TimeWheel(EventLoop * loop, int circle_num,int grid_num,int min_time_interval);
    ~TimeWheel();

    int getMaxLimit() { return min_time_interval_ * (grid_num_^circle_num_); }

    bool addTimer(const FuncCallback& cb,int after_time);

    void checkTimeWheel();

private:
    EventLoop * loop_;
    int circle_num_;
    int grid_num_;
    int min_time_interval_;
    uint64_t count_;

    typedef std::list<TimeData> TimeDataList;
    typedef std::vector<TimeDataList> TimeWheelData;
    struct TimeWheelListItem
    {
        TimeWheelListItem(int wheel_num):cur_pos(0),wheel(wheel_num){}
        int cur_pos;
        TimeWheelData wheel;
    };
    typedef std::vector<TimeWheelListItem> TimeWheelList;

    TimeWheelList time_wheel_list_;

    std::unique_ptr<Channel> channel;
};