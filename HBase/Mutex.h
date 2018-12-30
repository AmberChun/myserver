#pragma once

#include "base.h"

class MutexLock : public Nocopyable
{
public:
    MutexLock();
    ~MutexLock();

    void lock();
    void unlock();

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

