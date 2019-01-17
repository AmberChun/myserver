#include "EventLoop.h"
#include "Thread.h"
#include "Channel.h"
#include "Acceptor.h"
#include "TcpServer.h"
#include "Buffer.h"
#include "TcpClient.h"
#include "NoReturnThreadPool.h"
#include "ThreadPool.h"
#include "TcpConnection.h"
#include "TimeWheel.h"

#include <sys/timerfd.h>
#include <sys/syscall.h>
#include <arpa/inet.h>

//TFD_TIMER_ABSTIME

EventLoop* g_loop;
ThreadPool* g_pool;
NoReturnThreadPool* g_NoReturnPool;

void timeout()
{
	printf("Timeout!\n");
	//g_loop->quit();
}

void timeout2()
{
	printf("Timeout2!\n");
	//g_loop->quit();
}

void count()
{
	static int count = 0;
	printf("count:%d\n",++count);
}

void func()
{
	printf("Func run in threadId: %d\n",static_cast<pid_t>(::syscall(SYS_gettid)));
}

// void * threadFunc(void *)
// {
// 	printf("threadId: %d\n",static_cast<pid_t>(::syscall(SYS_gettid)));
// 	//EventLoop loop;
// 	//loop.loop();

// 	g_loop->runInLoop(func);
// 	g_loop->runAfter(timeout,2);


// 	return NULL;
// }

void acceptCallback(int connfd, const struct sockaddr_in&  peer_addr)
{
	printf("peer address: %s\n",inet_ntoa(peer_addr.sin_addr));
}

void messageCallback(const TcpConnectionPtr& connPtr,Buffer * buff)
{
	std::string str("");
	while( (str = buff->retrieveAsString()) != "")
	{
		printf("rev: %s\n",str.c_str());
		//g_NoReturnPool->AddNewTask([connPtr]{ connPtr->send("msg back\n");});
	}
}

void closeCallback(const TcpConnectionPtr& connPtr)
{
	printf("client disconnect connName:%s\n",connPtr->getName().c_str());
}

void connectCallback(const TcpConnectionPtr& connPtr)
{
	printf("client connect connName:%s\n",connPtr->getName().c_str());
}

void clientConnectCallback(const TcpConnectionPtr& connPtr)
{
	printf("connName:%s\n",connPtr->getName().c_str());
	// g_NoReturnPool->AddNewTask(
	// 	[connPtr]
	// 	{ 
	// 		for(int i = 0 ; i < 100; ++i)
	// 		{
	// 			char buf[64] = {0};
	// 			snprintf(buf,sizeof(buf),"msg back %d\n;",i);
	// 			connPtr->send(buf);
	// 			sleep(2);
	// 		}
	// 	}
	// );
}

void createThreadFunc(int index)
{
	for(int i = index * 100; i < (index + 1) * 100;++i)
	{
		char buf[32];
    	snprintf(buf,sizeof(buf),"client#%d",i);

		TcpClient client(buf,g_loop,"127.0.0.1",8888);
	// client.setMessageCallback(messageCallback);
	// client.setCloseCallback(closeCallback);
	// client.setConnectCallback(clientConnectCallback);
		client.connect();
	}
}

int main(int argc,char *argv[])
{
	EventLoop loop;
	g_loop = &loop;

	ThreadPool pool(5);
	g_pool = &pool;

	NoReturnThreadPool noreturnpool(4);
	g_NoReturnPool = &noreturnpool;
	//printf(" main threadId: %d\n",static_cast<pid_t>(::syscall(SYS_gettid)));

	// //struct timespec now;
	// //clock_gettime(CLOCK_MONOTONIC, &now);
	// struct itimerspec howlong;
	// memset(&howlong,0,sizeof(howlong));
	// howlong.it_interval.tv_sec = 2;
	// howlong.it_value.tv_sec = 2;
	// timerfd_settime(fd,0,&howlong,NULL);

	// loop.runAfter(timeout,10);
	//loop.runEvery(count,1);

	// Thread newthread(&threadFunc);
	// newthread.start();


	//TCP test----------------------------------------------------------
	if(argc == 2)
	{
		TcpServer server("firstServer",g_loop,8888);
		server.setMessageCallback(messageCallback);
		server.setCloseCallback(closeCallback);
		server.setConnectCallback(connectCallback);

		loop.loop();
	}
	else
	{
		clock_t start,end;

		start = clock();
		for(int i = 1; i <= 50;++i)
		{
			g_NoReturnPool->AddNewTask([i](){ createThreadFunc(i);}); 
		}

		end = clock();

		printf( "%f 秒\n", (double)(end - start) / CLOCKS_PER_SEC);

		//loop.loop();
	}
	//TCP test----------------------------------------------------------

	// TimeWheel wheel(g_loop,3,5,1);

	// wheel.addTimer(timeout2,10);

	loop.loop();

	return 0;
}
