#include "ThreadPool.h"

#include <iostream>

ThreadPool::ThreadPool(int threadNum, int bufSize)
    : _threadNum(threadNum), _bufSize(bufSize), _taskQueue(bufSize)
{
}

ThreadPool::~ThreadPool()
{
    stop();
}

// 启动线程池
void ThreadPool::start()
{
    for (int i = 0; i < _threadNum; ++i)
    {
        _vecThreads.push_back(std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this)));
    }

    for(auto &thread : _vecThreads)
    {
        thread->start();
    }
}

// 停止线程池
void ThreadPool::stop()
{
    _taskQueue.wakeup(); // 唤醒所有等待线程，防止死锁
    for (auto &thread : _vecThreads)
    {
        thread->join();
    }
    _vecThreads.clear();
}

// 添加任务
void ThreadPool::addTask(Task task)
{
    _taskQueue.push(task);
}

// 缓冲区中获取一个任务
TaskQueue::Task ThreadPool::getTask()
{
    return _taskQueue.pop();
}

// 线程池中每个线程的函数执行体
void ThreadPool::threadFunc()
{
    while (true)
    {
        Task task = getTask();
        if (!task)
        {
            break;
        }
        try {
            task();
        } catch(const std::exception &e) {
            std::cerr << "ThreadPool task exception: " << e.what() << std::endl;
        } catch(...) {
            std::cerr << "ThreadPool task unknown exception" << std::endl;
        }
    }
}