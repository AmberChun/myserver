#include "Epoller.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TimerQueue.h"

#include <sys/epoll.h>

static int MY_EPOLL_MOD = 0;// 0 为LT 或者 EPOLLET;

Epoller::Epoller(EventLoop * loop_)
:loop(loop_)
{
    epollfd = epoll_create(1);
}

Epoller::~Epoller()
{
    close(epollfd);
}

void Epoller::updateChannel(Channel * chl)
{
    if(NULL == chl)
    {
        return;
    }

    struct epoll_event et;
    et.data.fd = chl->getfd();
    et.events = chl->getevent() | MY_EPOLL_MOD;

    ChannelMap::iterator it = channelMap.find(chl->getfd());
    if(it == channelMap.end())
    {
        channelMap[et.data.fd] = chl;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, chl->getfd(), &et);
    }
    else
    {
        epoll_ctl(epollfd, EPOLL_CTL_MOD, chl->getfd(), &et);
    }

    if(channelMap.size() > eventList.size())
    {
        eventList.resize(channelMap.size() * 2);
    }
}

void Epoller::removeChannel(Channel * chl)
{
    if(NULL == chl)
    {
        return;
    }

    ChannelMap::iterator it = channelMap.find(chl->getfd());
    if(it != channelMap.end())
    {
        channelMap.erase(it);
    }

    epoll_ctl(epollfd, EPOLL_CTL_DEL, chl->getfd(), NULL);
}

Timestamp Epoller::runOnce(ChannelList* activeChannels)
{
    //memset(&eventList[0],0,(int)eventList.size());
    int nfds = epoll_wait(epollfd, &*eventList.begin(), (int)eventList.size(), 0);
    Timestamp now = loop->Now();
    if(nfds > 0)
    {
        fillActiveChannels(nfds,activeChannels);
    }

    return now;
}

 void Epoller::fillActiveChannels(int numEvents,ChannelList* activeChannels) const
 {
    for(int i = 0; i < numEvents; ++i)
    {
        ChannelMap::const_iterator it = channelMap.find(eventList[i].data.fd);
        if( it != channelMap.end())
        {
            it->second->setRevent(eventList[i].events);
            activeChannels->push_back(it->second);
        }
    }
 }