#include "TcpClient.h"
#include "Connector.h"
#include "TcpConnection.h"

using namespace std::placeholders;

TcpClient::TcpClient(const std::string& name_,EventLoop * loop_,const std::string& ip_,int port_)
:loop(loop_)
,tcpName(name_)
,connector(new Connector(loop_,ip_,port_,std::bind(&TcpClient::newConnection,this,_1,_2)))
{
    
}

TcpClient::~TcpClient()
{

}

 void TcpClient::newConnection(int connfd, const struct sockaddr_in&  peer_addr)
 {
     const struct sockaddr_in * plocaladdr = connector->getlocaladdr();
     connSPtr = std::make_shared<TcpConnection>(tcpName,loop,connfd,*plocaladdr,peer_addr);
     connSPtr->setConnectCallback(connectCallback);
     connSPtr->setMessageCallback(messageCallback);
     connSPtr->setCloseCallback(closeCallback);
     connSPtr->connectEstablished();
     connector->setConnected();
 }

void TcpClient::connect()
{
    connector->connect();
}

void TcpClient::send(const std::string& message)
{
    if(Connected())
    {
        connSPtr->send(message);
    }
}

bool TcpClient::Connected()
{
    return connector->Connected();
}