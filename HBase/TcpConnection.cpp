#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"

TcpConnection::TcpConnection(const std::string& name,EventLoop* loop_
                    ,int connfd_,const struct sockaddr_in& localaddr_,const struct sockaddr_in& peeraddr_)
:loop(loop_)
,connName(name)
,connChannel(new Channel(loop_,connfd_))
,localaddr(localaddr_)
,peeraddr(peeraddr_)
{
    connChannel->setReadCallback(std::bind(&TcpConnection::handleRead,this));
    connChannel->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    connChannel->setCloseCallback(std::bind(&TcpConnection::handleClose,this));
    connChannel->enableReading();
}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::handleRead()
{
    ssize_t n = inputBuff.readFd(connChannel->getfd());
    if(n > 0)
    {
        messageCallback(shared_from_this(),&inputBuff);
    }
    else if(n == 0)
    {
        handleClose();
    }
}

void TcpConnection::handleWrite()
{
    ssize_t n = ::write(connChannel->getfd(),outputBuff.head(), outputBuff.readableSize());

    if(n > 0)
    {
        outputBuff.retrieveString(n);
        if(outputBuff.readableSize() == 0)
        {
            connChannel->disableWriting();
        }
    }
}

void TcpConnection::handleClose()
{
    connChannel->disableAll();
    closeCallback(shared_from_this());
}

void TcpConnection::send(const std::string& message)
{
    if(loop->isInLoopThread())
    {
        sendInLoop(message);
    }
    else
    {
        loop->runInLoop(std::bind(&TcpConnection::sendInLoop,this,message));
    }
}

void TcpConnection::sendInLoop(const std::string& message)
{
    ssize_t n = 0;
    if(connChannel->isWriting() && outputBuff.readableSize() == 0)
    {
       n = ::write(connChannel->getfd(),message.data(),message.size());
    }

    if(static_cast<size_t>(n) < message.size())
    {
        outputBuff.append(message.data() + n,message.size() - n);
        if(!connChannel->isWriting())
        {
            connChannel->enableWriting();
        }
    }
}