#include "TcpConnection.h"
#include "Channel.h"

TcpConnection::TcpConnection(const std::string& name,EventLoop* loop_
                    ,int connfd_,const struct sockaddr_in& localaddr_,const struct sockaddr_in& peeraddr_)
:connName(name)
,connChannel(new Channel(loop_,connfd_))
,localaddr(localaddr_)
,peeraddr(peeraddr_)
{
    connChannel->setReadCallback(std::bind(&TcpConnection::handleRead,this));
    connChannel->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    connChannel->enableReading();
}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::handleRead()
{
    char buf[65535];
    ssize_t n = read(connChannel->getfd(),buf,sizeof(buf));
    messageCallback(shared_from_this(),buf,n);
}

void TcpConnection::handleWrite()
{

}