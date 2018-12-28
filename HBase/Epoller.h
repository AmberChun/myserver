#pragma once

#include "base.h"

class Channel;
class EventLoop;
class Epoller : public Nocopyable
{
public:
    typedef std::map<int,Channel*> ChannelMap;
    typedef std::vector<Channel*> ChannelList;
    typedef std::vector<struct epoll_event> EventList;

public:
    Epoller(EventLoop *);
    ~Epoller();

    void updateChannel(Channel * chl);
    void removeChannel(Channel * chl);

    //添加次函数是因为遍历eventList来处理事件的过程，如果对应的channel回调逻辑复杂，
    //那么执行之间会很长，这时候如果切换到其他线程然后对该Epoller对象调用updateChannel或者removeChannel，
    //会破坏原来的eventList
    void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;

    void runOnce(ChannelList* activeChannels);
private:
    EventLoop * loop;
    int epollfd;
    ChannelMap channelMap;
    EventList eventList;
};