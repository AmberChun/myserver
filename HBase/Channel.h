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

    void handleEvent(const Timestamp& now);

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

    void setCloseCallback(const EventCallback& cb)
    {
        closeCallback = cb;
    }

    void setIndex(int idx) { index = idx;}
    int getIndex() { return index;}
    int getfd() { return fd;}
    int getevent() { return event;}
    void setEvent(int ev) { event = ev; update();}

    void setRevent(int ev) { revent = ev;}

    //因为需要setCallback函数之后再把Channel加入到loop的循环中，所以得有这个函数
    void update();
    void disable();

    void enableReading() { event |= ReadEvent; update(); }
    void disableReading() { event &= ~ReadEvent; update(); }
    void enableWriting() { event |= WriteEvent; update(); }
    void disableWriting() { event &= ~WriteEvent; update(); }
    void disableAll() { event = 0; disable(); }
    bool isWriting() const { return event & WriteEvent; }
    bool isReading() const { return event & ReadEvent; }

    EventLoop* getLoop() { return loop;}
private:
    static const int NoneEvent;
    static const int ReadEvent;
    static const int WriteEvent;
    
    
    EventLoop*  loop;
    const int   fd;
    int         event;
    int         revent;
    int         index;

    EventCallback readCallback;
    EventCallback writeCallback;
    EventCallback errorCallback;
    EventCallback closeCallback;
};