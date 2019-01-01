#pragma once

#include "base.h"

class Channel;
class EventLoop;
class Acceptor : public Nocopyable
{
public:
public:
    Acceptor(EventLoop * loop_,int port_,const NewConnectionCallback& cb_,int maxConnNum_ = 100);
    ~Acceptor();

    int getPort() { return port; }

    void handleRead();

    const struct sockaddr_in * getlocaladdr() { return &localaddr;}

private:
    int createListenFd(int port_);
private:
    int port;
    int maxConnNum;
    std::unique_ptr<Channel> acceptChannel;
    NewConnectionCallback cb;
    struct sockaddr_in localaddr;
};