#pragma once

#include "base.h"

class Thread : public Nocopyable
{
public:
typedef void* (*ThreadFunc)(void*);

public:
	Thread(ThreadFunc);
	~Thread();

	void start();
	int join();

private:
	pthread_t tid;
	ThreadFunc func;
	bool started;
	bool joined;
};
