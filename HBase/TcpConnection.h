#pragma once

#include "base.h"
#include "Buffer.h"

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
    void handleClose();

    void setMessageCallback(const MessageCallback& cb) { messageCallback = cb;}
    void setConnectCallback(const ConnectCallback& cb) { connectCallback = cb;}
    void setCloseCallback(const CloseCallback& cb) { closeCallback = cb;}

    //保证线程安全，如果在loop线程中，那么直接执行，如果不在，则加入loop的执行函数中
    void send(const std::string& message);
    void sendInLoop(const std::string& message);

    void connectEstablished();
private:
    enum ConnState
    {
        kConnecting = 0,
        kConnected = 1,
    };

    EventLoop * loop;
    int state;
    std::string connName;
    std::unique_ptr<Channel> connChannel;
    struct sockaddr_in localaddr;
    struct sockaddr_in peeraddr;

    MessageCallback messageCallback;
    ConnectCallback connectCallback;
    CloseCallback closeCallback;

    Buffer inputBuff;
    Buffer outputBuff;
};