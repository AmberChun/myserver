#include "Connector.h"
#include "EventLoop.h"
#include <sys/socket.h> //使用套接字主要几个函数
#include <arpa/inet.h>  //使用inet_ntop这一类的地址转换函数

Connector::Connector(EventLoop * loop_,const std::string& ip_, int server_port_,const NewConnectionCallback& cb_)
:loop(loop_)
,server_port(server_port_)
,ip(ip_)
,cb(cb_)
,retrydelayMs(0)
,connState(CS_NO_CONNECT)
{
    memset(&localaddr,0,sizeof localaddr);
    memset(&servaddr,0,sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port_);
    inet_pton(AF_INET,ip.c_str(),&servaddr.sin_addr);
}

Connector::~Connector()
{

}

void Connector::connect()
{
    int sockfd = ::socket(AF_INET,SOCK_STREAM,0);
    int ret = ::connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr));
    int savedErrno = (ret == 0) ? 0 : errno;
    switch(savedErrno)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            {
                socklen_t len = sizeof localaddr;
                ::getsockname(sockfd,(struct sockaddr *)&localaddr,&len);
                cb(sockfd,servaddr);
            }
            break;
        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            {
                ::close(sockfd);
                retry();
            }
            break;
        default:
            ::close(sockfd);
            break;
    }
}

void Connector::retry()
{
    retrydelayMs = retrydelayMs == 0 ? 1: (retrydelayMs * 2 > MAX_DELAY_TIME ?  MAX_DELAY_TIME : retrydelayMs * 2);
    loop->runAfter(std::bind(&Connector::connect,this),retrydelayMs);
}