#pragma once

#include "base.h"

//一个TcpClient只持有一个连接

class EventLoop;
class Connector;
class TcpClient: Nocopyable
{
public:
    TcpClient(const std::string& name_,EventLoop * loop_,const std::string& ip_,int prot_);
    ~TcpClient();

    void newConnection(int connfd, const struct sockaddr_in&  peer_addr);

    void setMessageCallback(const MessageCallback& cb) { messageCallback = cb;}
    void setConnectCallback(const ConnectCallback& cb) { connectCallback = cb;}
    void setCloseCallback(const CloseCallback& cb) { closeCallback = cb;}
    
    void connect();
    void send(const std::string& message);

    bool Connected();
private:
    EventLoop * loop;
    const std::string tcpName;
    std::unique_ptr<Connector> connector;
    TcpConnectionPtr connSPtr;

    MessageCallback messageCallback;
    ConnectCallback connectCallback;
    CloseCallback closeCallback;

};