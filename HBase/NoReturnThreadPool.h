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

    template<class F, class...Args>
    void AddNewTask(F&& f,Args&&... args);

private:
    std::vector<std::thread> threadList;
    std::deque<Task> taskList;
    bool stop;
    MutexLock lock;
    pthread_cond_t cond;
};