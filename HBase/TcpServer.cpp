#include "TcpServer.h"
#include "Acceptor.h"
#include "TcpConnection.h"

using namespace std::placeholders;

TcpServer::TcpServer(const std::string& name_,EventLoop * loop_,int port_)
:loop(loop_)
,tcpName(name_)
,acceptor(new Acceptor(loop_,port_,std::bind(&TcpServer::newConnection,this,_1,_2)))
,nextConnId(0)
{
 
}

TcpServer::~TcpServer()
{

}

 void TcpServer::newConnection(int connfd, const struct sockaddr_in&  peer_addr)
 {
     char buf[32];
     snprintf(buf,sizeof(buf),"#%d",nextConnId);
     ++nextConnId;
     std::string connName = tcpName + buf;

     const struct sockaddr_in * plocaladdr = acceptor->getlocaladdr();
     TcpConnectionPtr connSPtr = std::make_shared<TcpConnection>(connName,loop,connfd,*plocaladdr,peer_addr);
     connMap[connName] = connSPtr;
     connSPtr->setConnectCallback(connectCallback);
     connSPtr->setMessageCallback(messageCallback);
 }