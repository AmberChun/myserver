#pragma once

#include <vector>
#include <map>
#include <set>
#include <functional>
#include <string>
#include "Nocopyable.h"
#include <memory.h>
#include <memory>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <netinet/in.h> //使用sockaddr_in

typedef int64_t Timestamp;

typedef std::function<void (int connfd, const struct sockaddr_in&  peer_addr)> NewConnectionCallback;

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void (const TcpConnectionPtr& connPtr)> ConnectCallback;
typedef std::function<void (const TcpConnectionPtr& connPtr, char *buffer,int bufsize)> MessageCallback;



