#pragma once

#include "base.h"
#include "Mutex.h"
#include "TimerQueue.h"

class Channel;
class Epoller;
class MutexLock;
class TimerQueue;
class EventLoop : public Nocopyable
{
public:
    typedef std::vector<Channel *> ChannelList;
    typedef std::function<void ()> FuncCallback;
    typedef std::vector<FuncCallback> FuncCallbackList;
public:
    EventLoop();
    ~EventLoop();

    static EventLoop* getEventLoopOfCurrentThread();
    
    void loop();
    void quit();

    //channel相关
    void updateChannel(Channel * channel);

    //其他线程可调用
    void runInLoop(const FuncCallback& cb);

    void runAt(const FuncCallback& cb,Timestamp when);
    void runEvery(const FuncCallback& cb,int interval);
private:
    bool isInLoopThread();
    //此调用中不得再往funcCallbackList容器中push新的内容
    void runFuncCallback();


private:
    bool isLooping;
    bool quit_;
    const pid_t threadId;
    std::unique_ptr<Epoller> epoller;
    ChannelList activeChannels;

    FuncCallbackList funcCallbackList;
    MutexLock mutex;

    TimerQueue queue;
};
