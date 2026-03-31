#include "TaskQueue.h"

TaskQueue::TaskQueue(size_t queSize)
    : _mutex()
    , _notEmpty(_mutex)
    , _notFull(_mutex)
    , _queSize(queSize)
    , _que()
    , _flag(true)
{
}

TaskQueue::~TaskQueue()
{
}

// 判断队列是否为空
bool TaskQueue::empty() const
{
    return _que.size() == 0;
}

// 判断队列是否已满
bool TaskQueue::full() const
{
    return _que.size() == _queSize;
}

// 缓冲区添加任务
void TaskQueue::push(const Task &task)
{
    //_mutex.lock();
    MutexLockGuard lock(_mutex); // 使用RAII机制自动加锁和解锁
    while(full() && _flag)
    {
        _notFull.wait();
    }
    if(!_flag)
    {
        return;
    }
    _que.push(task);
    _notEmpty.notify();
    //_mutex.unlock();
}

// 缓冲区获取任务
TaskQueue::Task TaskQueue::pop()
{
    //_mutex.lock();
    MutexLockGuard lock(_mutex); // 使用RAII机制自动加锁和解锁
    while(empty() && _flag)
    {
        _notEmpty.wait();
    }
    if(!_flag)
    {
        return nullptr; // 返回空指针表示没有任务可获取
    }
    Task task = _que.front();
    _que.pop();
    _notFull.notify();
    //_mutex.unlock();
    return task;
}

// 唤醒_notEmpty条件变量
void TaskQueue::wakeup()
{
    MutexLockGuard lock(_mutex); // 使用RAII机制自动加锁和解锁
    _flag = false; // 设置标志位为false，表示不再等待任务
    _notEmpty.notifyAll();
    _notFull.notifyAll();
}