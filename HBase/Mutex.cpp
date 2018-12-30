#include "Mutex.h"

MutexLock::MutexLock()
:mutex(PTHREAD_MUTEX_INITIALIZER)
{

}

MutexLock::~MutexLock()
{
    pthread_mutex_destroy(&mutex);
}

void MutexLock::lock()
{
    pthread_mutex_lock(&mutex);
}

void MutexLock::unlock()
{
    pthread_mutex_unlock(&mutex);
}


MutexLockGuard::MutexLockGuard(MutexLock& ml)
: mutex_lock(ml)
{
    mutex_lock.lock();
}

MutexLockGuard::~MutexLockGuard()
{
    mutex_lock.unlock();
}