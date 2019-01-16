#pragma once

#include "base.h"
#include "Mutex.h"

//先做一个最低精度为秒的定时器

typedef std::function<void ()> TimerCallback;
typedef uint64_t TimeID;

class EventLoop;
class Channel;
class Timer //: public Nocopyable
{
public:
    Timer(TimerCallback cb_, Timestamp when,Timestamp interval_,int countMax_ = 0);
    ~Timer();

    void runCallback();

    Timestamp restart(Timestamp now);

    bool repeated();

    void Disable() {disable = true;}
private:
    TimerCallback cb;
    Timestamp expiration;   //时间戳
    const Timestamp interval;  //时间间隔
    const bool repeat;      //是否重复
    int count;              //重复次数
    int countMax;           //最高重复次数

    bool disable;           //取消定时器用
};

class TimerQueue : public Nocopyable
{
public:
    typedef std::pair<Timestamp,TimerSPtr> Entry;
    typedef std::vector<Entry> ActiveTimerList;
    typedef std::vector<Entry> RecycleTimerList;
    typedef std::multimap<Timestamp,TimerSPtr> TimerMultiMap;
public:
    static Timestamp Now();

public:
    TimerQueue(EventLoop *loop_);
    ~TimerQueue();

public:
    TimerWPtr addTimer(const TimerCallback& cb,Timestamp when,Timestamp interval,int countMax_ = 0);

    void update();

    void doActiveTimer();

    void recycleTimer();
private:
    std::unique_ptr<Channel> timerChannel;
    TimerMultiMap timerMap;
    ActiveTimerList activeTimerList;
    RecycleTimerList recycleTimerList;

    std::vector<TimeID> cancelList;

    MutexLock mutex;
};