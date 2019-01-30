#include "Thread.h"
#include <assert.h>
#include <stdlib.h>

Thread::Thread(ThreadFunc f)
:tid(0),
 func(f),
 started(false),
 joined(false)
{

}

Thread::~Thread()
{
	if(started && !joined)
	{
		pthread_detach(tid);
	}
}

void Thread::start()
{
	assert(!started);
	started = true;
	
	if(pthread_create(&tid,NULL,func,NULL))
	{
		exit(0);
	}
	else
	{
		assert(tid > 0);
	}
}

int Thread::join()
{
	assert(started);
	assert(!joined);
	joined = true;
	return pthread_join(tid,NULL);	
}
