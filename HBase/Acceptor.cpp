#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"

#include <sys/socket.h> //使用套接字主要几个函数
//#include <unistd.h>     //使用fork、getpid、close函数
#include <arpa/inet.h>  //使用inet_ntop这一类的地址转换函数

Acceptor::Acceptor(EventLoop * loop_,int port_,const NewConnectionCallback& cb_,int maxConnNum_)
:port(port_)
,maxConnNum(maxConnNum_)
,acceptChannel(new Channel(loop_,createListenFd(port_)))
,cb(cb_)
{
    memset(&localaddr,0,sizeof localaddr);
    acceptChannel->setReadCallback(std::bind(&Acceptor::handleRead,this));
    acceptChannel->enableReading();
}

int Acceptor::createListenFd(int port_)
{
    int listenfd;
    listenfd = socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC ,0);
    
    memset(&localaddr,0,sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localaddr.sin_port = htons(port_);
    
    if(0 != bind(listenfd,(sockaddr *)&localaddr,sizeof(localaddr)))
    {
        printf("bind port: %d fail!\n",port_);
        exit(0);
    }
    
    listen(listenfd,maxConnNum);

    return listenfd;
}

Acceptor::~Acceptor()
{
    ::close(acceptChannel->getfd());
}

void Acceptor::handleRead()
{
    sockaddr_in peeraddr;
    unsigned int peerlen = sizeof(peeraddr);
    int connfd = accept(acceptChannel->getfd(), (sockaddr *)&peeraddr, &peerlen);
    if(connfd >= 0)
    {
        if(cb)
        {
            cb(connfd,peeraddr);
        }
        else
        {
            close(connfd);
        }
    }
}
