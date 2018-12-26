#pragma once

#include <unistd.h> //syscall
#include <sys/syscall.h> //syscall‘s param
#include <pthread.h>
#include "Nocopyable.h"

static pid_t gettid()
{
	return static_cast<pid_t>(::syscall(SYS_gettid));
}



class EventLoop : public Nocopyable
{
public:
    EventLoop();
    ~EventLoop();

    bool isInLoopThread() { return threadId == gettid();}

    static EventLoop* getEventLoopOfCurrentThread();
    
    void loop();


private:
    bool isLooping;
    const pid_t threadId;
};
