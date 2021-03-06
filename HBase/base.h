#pragma once

#include <vector>
#include <map>
#include <set>
#include <deque>
#include <list>
#include <functional>
#include <string>
#include "Nocopyable.h"
#include <memory.h>
#include <memory>
#include <pthread.h>
#include <sys/time.h>
#include <netinet/in.h> //使用sockaddr_in
#include <assert.h> //assert
#include <stdlib.h> //exit
#include <stdio.h> //printf
#include <unistd.h> //syscall\fork、getpid、close
#include <sys/syscall.h> //syscall‘s param
#include <sys/eventfd.h> //eventfd
#include <time.h>

typedef int64_t Timestamp;

typedef std::function<void (int connfd, const struct sockaddr_in&  peer_addr)> NewConnectionCallback;

class TcpConnection;
class Buffer;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void (const TcpConnectionPtr& connPtr)> ConnectCallback;
typedef std::function<void (const TcpConnectionPtr& connPtr)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr& connPtr, Buffer * buff)> MessageCallback;

#define MAX_DELAY_TIME 512

class Timer;
typedef std::shared_ptr<Timer> TimerSPtr;
typedef std::weak_ptr<Timer> TimerWPtr;

typedef std::function<void()> FuncCallback;



