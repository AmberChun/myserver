#include "TimeWheel.h"
#include "EventLoop.h"
#include "Channel.h"
#include <sys/timerfd.h>

TimeWheel::TimeWheel(EventLoop * loop, int circle_num,int grid_num,int min_time_interval)
    :loop_(loop)
    ,circle_num_(circle_num)
    ,grid_num_(grid_num)
    ,min_time_interval_(min_time_interval)
    ,count_(0)
    ,channel(new Channel(loop_,::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC)))
{
    struct itimerspec howlong;
    memset(&howlong,0,sizeof(howlong));
    howlong.it_value.tv_sec = 1;
    howlong.it_interval.tv_sec = 1;
    
    ::timerfd_settime(channel->getfd(),0,&howlong,NULL);

    channel->setReadCallback(std::bind(&TimeWheel::checkTimeWheel,this));
    channel->enableReading();

    for(int i = 0; i < circle_num_;++i)
    {
        TimeWheelListItem item(grid_num_);
        time_wheel_list_.push_back(grid_num_);
    }
}

TimeWheel::~TimeWheel()
{
    channel->disableAll();
}

bool TimeWheel::addTimer(const FuncCallback& cb,int after_time)
{
    if(!loop_->isInLoopThread())
    {
        return false;
    }

    // if(after_time > getMaxLimit())
    // {
    //     return false;
    // }

    for(int i = circle_num_ - 1; i >= 0 ;--i)
    {
        int interval = min_time_interval_;   
        for(int j = 0; j < i; ++j)
        {
            interval *= grid_num_;
        }

        if(after_time >= interval || (i == 0 && after_time < interval))
        {
            int64_t rest = count_ % interval ;
            after_time += rest;
            int val = after_time / interval - 1;

            TimeData data;
            data.time = after_time % interval;  //这个操作可以不放在这，看后面怎么修改
            data.cb = cb;
            time_wheel_list_[i].wheel[(time_wheel_list_[i].cur_pos + val) % grid_num_].push_back(data);
            
            return true;
        }
        
    }

    return false;
}

void TimeWheel::checkTimeWheel()
{
    uint64_t val = 0;
    ssize_t s = read(channel->getfd(),&val,sizeof(val));
    if(s <= 0)
    {
        return;
    }

    
    uint64_t old_count = count_;
    count_ += val;
    for(int index = 0; index < circle_num_ ;++index)
    {
        int interval = min_time_interval_;   
        for(int j = 0; j < index; ++j)
        {
            interval *= grid_num_;
        }

        uint64_t rest = old_count % interval ;
        if(rest + val >= static_cast<uint64_t>(interval))
        { 
            TimeDataList& list = time_wheel_list_[index].wheel[time_wheel_list_[index].cur_pos];
            for(TimeDataList::iterator it = list.begin();it != list.end();++it)
            {
                if(it->time <= 0)
                {
                    it->cb();
                    //printf("index:%d  pos:%d  callback\n",index,time_wheel_list_[index].cur_pos);
                }
                else
                {
                    addTimer(it->cb,it->time);
                    //printf("index:%d  pos:%d  time:%d addtimer\n",index,time_wheel_list_[index].cur_pos,it->time);
                }
            }

            list.clear();

            time_wheel_list_[index].cur_pos = (time_wheel_list_[index].cur_pos + 1) % grid_num_;
           
        }
    }

    //printf("count:%lu\n",count_);
}