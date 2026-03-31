#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__ 

#include "MutexLock.h"
#include "Condition.h"
#include <functional>
#include <queue>

class TaskQueue {
public:
    typedef std::function<void()> Task; //任务的回调函数作为接口

public:
    TaskQueue(int); //构造函数
    void empty();   //判断队列是否为空
    void full();    //判断队列是否已满
    void push(Task);    //缓冲区添加任务
    Task pop();     //缓冲区获取任务
    void wakeup();  //唤醒_notEmpty条件变量

private:
    MutexLock _mutex;   //互斥锁对象
    Condition _notEmpty;    //缓冲区有数据的条件变量
    Condition _notFull;     //缓冲区没有存满数据的条件变量
    size_t _size;   //队列大小
    std::queue<Task> _que;  //任务队列
    bool _flag; //标志位，用于推出pop函数
};

#endif