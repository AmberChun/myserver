#include "EventLoop.h"
#include "Thread.h"

void * threadFunc(void *)
{
	EventLoop loop;
	loop.loop();

	return NULL;
}


int main()
{
	EventLoop loop;

	Thread thread(&threadFunc);
	thread.start();
	loop.loop();

	
	pthread_exit(NULL);
	return 0;
}
