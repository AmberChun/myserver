#pragma once

#include "base.h"

class MutexLock : public Nocopyable
{
public:
    MutexLock();
    ~MutexLock();

    void lock();
    void unlock();

    pthread_mutex_t * getmutex() { return &mutex;}

private:
    pthread_mutex_t mutex;
};

class MutexLockGuard : public Nocopyable
{
public:
    MutexLockGuard(MutexLock& ml);
    ~MutexLockGuard();

private:
    MutexLock& mutex_lock;
};

