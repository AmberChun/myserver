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

private:
    bool isLooping;
    const pid_t threadId;
    std::unique_ptr<Epoller> epoller;
    ChannelList activeChannels;
};
