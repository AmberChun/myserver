#pragma once

#include "base.h"
#include "Mutex.h"

class Channel;
class Epoller;
class MutexLock;
class TimerQueue;
class EventLoop : public Nocopyable
{
  public:
    typedef std::vector<Channel *> ChannelList;
    typedef std::function<void()> FuncCallback;
    typedef std::vector<FuncCallback> FuncCallbackList;

  public:
    EventLoop();
    ~EventLoop();

    static EventLoop *getEventLoopOfCurrentThread();

    void loop();
    void quit();

    //channel相关
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    //其他线程可调用
    void runInLoop(const FuncCallback &cb);

    TimerWPtr runAfter(const FuncCallback &cb, Timestamp when);
    TimerWPtr runEvery(const FuncCallback &cb, int interval);

    bool isInLoopThread();

    Timestamp getTimeCache();// 获取时间缓存
    Timestamp Now() { return getTimeCache();}

  private:
    
    //此调用中不得再往funcCallbackList容器中push新的内容
    void runFuncCallback();

    void wakeup();     //runInLoop 调用
    void handleRead(); //wakeupChannel 的读事件调用

    void updateTimeCache();//更新时间缓存
    void clearTimeCache();//清空时间缓存
   
  private:
    bool isLooping;
    bool quit_;
    const pid_t threadId;
    std::unique_ptr<Epoller> epoller;
    ChannelList activeChannels;

    FuncCallbackList funcCallbackList;
    MutexLock mutex;

    std::unique_ptr<TimerQueue> timerQueue;

    //EventLoop的唤醒功能
    std::unique_ptr<Channel> wakeupChannel;

    //时间缓存
    Timestamp time_cache_;
};
