#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__ 

#include <vector>
#include <functional>
#include <memory>

#include "Thread.h"
#include "TaskQueue.h"
#include "../base/Noncopyable.h"

class ThreadPool : NonCopyable
{
public:
    typedef std::function<void()> Task;  //重定义回调函数的标签

public:
    ThreadPool(int threadNum, int bufSize);
    ~ThreadPool();
    void start();   // 启动线程池
    void stop();    // 停止线程池
    void addTask(Task task); // 添加任务
    void threadFunc();  //线程池中每个线程的函数执行体

private:
    Task getTask(); // 缓冲区中获取一个任务

private:
    int _threadNum; // 线程数
    std::vector<std::unique_ptr<Thread>> _vecThreads;   //线程对象的容器
    int _bufSize;   //缓冲区大小
    TaskQueue _taskQueue;   //缓冲区对象

};

#endif