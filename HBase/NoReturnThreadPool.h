#pragma once

#include "base.h"
#include <thread>
#include "Mutex.h"


class NoReturnThreadPool : public Nocopyable
{
public:
    typedef std::function<void()> Task;
public:
    NoReturnThreadPool(int threadNum_);
    ~NoReturnThreadPool();

    template<class F, class... Args>
    void AddNewTask(F&& f,Args&&... args);

private:
    std::vector<std::thread> threadList;
    std::deque<Task> taskList;
    bool stop;
    MutexLock lock;
    pthread_cond_t cond;
};

template<class F, class... Args>
void NoReturnThreadPool::AddNewTask(F&& f,Args&&... args)
{
    Task task = std::bind(std::forward<F>(f),std::forward<Args>(args)...);

    {
        MutexLockGuard guard(lock);
        if(stop)
        {
            return;
        }
        taskList.emplace_back(task);
    }

    pthread_cond_signal(&cond);
}

NoReturnThreadPool::NoReturnThreadPool(int threadNum_)
:stop(false)
,cond(PTHREAD_COND_INITIALIZER)
{
    for(int i = 0; i < threadNum_;++i)
    {
        threadList.emplace_back(
            [this]
            {
                for(;;)
                {
                    Task task;

                    {
                        MutexLockGuard guard(lock);
                        while(!stop && taskList.empty())
                        {
                            pthread_cond_wait(&cond,lock.getmutex());
                        }
                            
                        if(stop)
                        {
                            return;
                        }

                        task = std::move(taskList.front());
                        taskList.pop_front();
                    }

                    task();
                }
            }
        );
    }
}

NoReturnThreadPool::~NoReturnThreadPool()
{
    {
        MutexLockGuard guard(lock);
        stop = true;
    }

    pthread_cond_broadcast(&cond);
    for(std::thread &thd: threadList)
        thd.join();
}
