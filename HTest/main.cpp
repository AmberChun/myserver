#include "EventLoop.h"
#include "Thread.h"
#include "Channel.h"
#include "Acceptor.h"
#include "TcpServer.h"
#include "Buffer.h"
#include "TcpClient.h"

#include <sys/timerfd.h>
#include <sys/syscall.h>
#include <arpa/inet.h>

//TFD_TIMER_ABSTIME

EventLoop* g_loop;

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
	}
}

void closeCallback(const TcpConnectionPtr& connPtr)
{
	printf("peer disconnect\n");
}

void connectCallback(const TcpConnectionPtr& connPtr)
{
	printf("client connect\n");
}

int main(int argc,char *argv[])
{
	EventLoop loop;
	g_loop = &loop;

	//printf(" main threadId: %d\n",static_cast<pid_t>(::syscall(SYS_gettid)));

	// int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	// Channel chl(&loop,fd);
	// chl.setReadCallback(timeout);
	// chl.enableReading();

	// //struct timespec now;
	// //clock_gettime(CLOCK_MONOTONIC, &now);
	// struct itimerspec howlong;
	// memset(&howlong,0,sizeof(howlong));
	// howlong.it_interval.tv_sec = 2;
	// howlong.it_value.tv_sec = 2;
	// timerfd_settime(fd,0,&howlong,NULL);

	//loop.runEvery(timeout,2);

	// Thread newthread(&threadFunc);
	// newthread.start();

	//Acceptor acceptor(g_loop,8888,acceptCallback);
	if(argc == 2)
	{
		TcpServer server("firstServer",g_loop,atoi(argv[1]));
		server.setMessageCallback(messageCallback);
		server.setCloseCallback(closeCallback);
		server.setConnectCallback(connectCallback);

		loop.loop();
	}
	else
	{
		TcpClient client("firstClient",g_loop,"127.0.0.1",8888);
		client.setMessageCallback(messageCallback);
		client.setCloseCallback(closeCallback);
		client.connect();


		loop.loop();
	}

	return 0;
}
