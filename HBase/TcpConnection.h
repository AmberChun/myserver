#pragma once

#include "base.h"

class Channel;
class EventLoop;
class TcpConnection : public Nocopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(const std::string& name,EventLoop* loop_
                    ,int connfd_,const struct sockaddr_in& localaddr_,const struct sockaddr_in& peeraddr_);
    ~TcpConnection();

    void setState(int state_) { state = state_;}

    void handleRead();
    void handleWrite();

    void setMessageCallback(const MessageCallback& cb) { messageCallback = cb;}
    void setConnectCallback(const ConnectCallback& cb) { connectCallback = cb;}

private:
    enum ConnState
    {
        kConnecting = 0,
        kConnected = 1,

    };

    int state;
    std::string connName;
    std::unique_ptr<Channel> connChannel;
    struct sockaddr_in localaddr;
    struct sockaddr_in peeraddr;

    MessageCallback messageCallback;
    ConnectCallback connectCallback;
};