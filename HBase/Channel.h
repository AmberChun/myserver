#pragma once

#include "base.h"

class EventLoop;

class Channel : public Nocopyable
{
public:
typedef std::function<void ()> EventCallback;

public:
    Channel(EventLoop *loop_,int fd_);
    ~Channel();

    void handleEvent();

    void setReadCallback(const EventCallback& cb)
    {
        readCallback = cb;
    }

    void setWriteCallback(const EventCallback& cb)
    {
        writeCallback = cb;
    }

    void setErrorCallback(const EventCallback& cb)
    {
        errorCallback = cb;
    }

    void setIndex(int idx) { index = idx;}
    int getIndex() { return index;}
    int getfd() { return fd;}
    int getevent() { return event;}
private:

    EventLoop*  loop;
    const int   fd;
    int         event;
    int         index;

    EventCallback readCallback;
    EventCallback writeCallback;
    EventCallback errorCallback;
};