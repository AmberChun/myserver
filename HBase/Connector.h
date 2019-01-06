#pragma once

#include "base.h"

class EventLoop;
class Connector : public Nocopyable
{
public:
    enum ConnectState
    {
        CS_NO_CONNECT = 0,  //未连接
        CS_CONNECTED = 1,   //已连接
    };
public:
    Connector(EventLoop * loop_,const std::string& ip_,int server_port_,const NewConnectionCallback& cb_);
    ~Connector();

    void connect();

    const struct sockaddr_in * getlocaladdr() { return &localaddr;}

    bool Connected() { return connState == CS_CONNECTED;}

    void setConnected() { connState = CS_CONNECTED;}

private:
    void connecting(int sockfd);

    void retry();
private:
    EventLoop * loop;
    int server_port;
    const std::string ip;
    NewConnectionCallback cb;
    struct sockaddr_in servaddr;
    struct sockaddr_in localaddr;
    Timestamp retrydelayMs;

    int connState;
};