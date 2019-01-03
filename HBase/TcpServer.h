#pragma once

#include "base.h"

class Acceptor;
class EventLoop;
class TcpServer : public Nocopyable
{
public:
    typedef std::map<std::string,TcpConnectionPtr> ConnMap;
public:
    TcpServer(const std::string& name_,EventLoop * loop_,int port_);
    ~TcpServer();

    void newConnection(int connfd, const struct sockaddr_in&  peer_addr);

    void setMessageCallback(const MessageCallback& cb) { messageCallback = cb;}
    void setConnectCallback(const ConnectCallback& cb) { connectCallback = cb;}
    void setCloseCallback(const CloseCallback& cb) { closeCallback = cb;}
    
private:
    EventLoop * loop;
    const std::string tcpName;
    std::unique_ptr<Acceptor> acceptor;
    ConnMap connMap;
    int nextConnId;

    MessageCallback messageCallback;
    ConnectCallback connectCallback;
    CloseCallback closeCallback;
};