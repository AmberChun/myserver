#pragma once

#include "base.h"

class Channel;
class Epoller;
class EventLoop : public Nocopyable
{
public:
    typedef std::vector<Channel *> ChannelList;
public:
    EventLoop();
    ~EventLoop();

    bool isInLoopThread();

    static EventLoop* getEventLoopOfCurrentThread();
    
    void loop();
    void quit();

    //channel相关
    void updateChannel(Channel * channel);

private:
    bool isLooping;
    bool quit_;
    const pid_t threadId;
    std::unique_ptr<Epoller> epoller;
    ChannelList activeChannels;
};
