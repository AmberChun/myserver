#pragma once

#include "base.h"
#include "Channel.h"
#include "Mutex.h"

//先做一个最低精度为秒的定时器

typedef std::function<void ()> TimerCallback;

class Timer //: public Nocopyable
{
public:
    Timer(TimerCallback cb_, Timestamp when,Timestamp interval_,int countMax_ = 0);
    ~Timer();

    void runCallback();

    Timestamp restart(Timestamp now);

    bool repeated();
private:
    TimerCallback cb;
    Timestamp expiration;   //时间戳
    const Timestamp interval;  //时间间隔
    const bool repeat;      //是否重复
    int count;              //重复次数
    int countMax;           //最高重复次数
};

class TimerQueue : public Nocopyable
{
public:
    typedef std::pair<Timestamp,Timer> Entry;
    typedef std::vector<Entry> ActiveTimerList;
    typedef std::vector<Entry> RecycleTimerList;
    typedef std::multimap<Timestamp,Timer> TimerMultiMap;

public:
    TimerQueue(EventLoop *loop_);
    ~TimerQueue();

public:
    void addTimer(const TimerCallback& cb,Timestamp when,Timestamp interval,int countMax_ = 0);

    void update();

    void doActiveTimer();

    void recycleTimer();
private:
    Channel timerChannel;
    TimerMultiMap timerMap;
    ActiveTimerList activeTimerList;
    RecycleTimerList recycleTimerList;

    MutexLock mutex;
};