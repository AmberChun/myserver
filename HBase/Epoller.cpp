#include "Epoller.h"
#include "Channel.h"
#include "EventLoop.h"

#include <sys/epoll.h>

static int MY_EPOLL_MOD = EPOLLET;

Epoller::Epoller(EventLoop * loop_)
:loop(loop_)
{
    epollfd = epoll_create(1);
}

Epoller::~Epoller()
{

}

void Epoller::updateChannel(Channel * chl)
{
    if(NULL == chl)
    {
        return;
    }

    ChannelMap::iterator it = channelMap.find(chl->getfd());
    if(it == channelMap.end())
    {
        struct epoll_event et;
        et.data.fd = chl->getfd();
        et.events = chl->getevent() | MY_EPOLL_MOD;
        channelMap[et.data.fd] = chl;
        eventList.push_back(et);
        chl->setIndex((int)eventList.size() - 1);

        epoll_ctl(epollfd, EPOLL_CTL_ADD, chl->getfd(), &et);
    }
    else
    {
        eventList[chl->getIndex()].events = chl->getevent() | MY_EPOLL_MOD;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, chl->getfd(), &eventList[chl->getIndex()]);
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

    std::vector<struct epoll_event>::iterator iter = eventList.begin();
    while(iter != eventList.end())
    {
        if(iter->data.fd == chl->getfd())
        {
            eventList.erase(iter);
            break;
        }
    }

    epoll_ctl(epollfd, EPOLL_CTL_DEL, chl->getfd(), NULL);
}

void Epoller::runOnce(ChannelList* activeChannels)
{
    int nfds = epoll_wait(epollfd, &eventList[0], (int)eventList.size(), 0);
    if(nfds > 0)
    {
        fillActiveChannels(nfds,activeChannels);
    }
}

 void Epoller::fillActiveChannels(int numEvents,ChannelList* activeChannels) const
 {
    for(int i = 0; i < numEvents; ++i)
    {
        ChannelMap::const_iterator it = channelMap.find(eventList[i].data.fd);
        if( it != channelMap.end())
        {
            activeChannels->push_back(it->second);
        }
    }
 }