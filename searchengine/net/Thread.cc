#include "Thread.h"

#include <iostream>

#include <stdio.h>
#include <unistd.h>

using std::cout;
using std::endl;

// 构造函数
Thread::Thread(ThreadCallback &&cb)
    : _pthId(0)
    , _isRunning(false)
    , _cb(std::move(cb))
{

}
Thread::~Thread()
{
    if(_isRunning) {
        pthread_detach(_pthId);
    }
}

// 启动线程
void Thread::start()
{
    if(_isRunning) {
        return;
    }
    int ret = pthread_create(&_pthId, nullptr, threadFunc, this);
    if (ret != 0) {
        // 创建线程失败
        perror("pthread_create");
        return;
    }
    _isRunning = true;
}

// 等待线程结束
void Thread::join()
{
    if(!_isRunning) {
        return;
    }
    int ret = pthread_join(_pthId, nullptr);
    if (ret != 0) {
        // 等待线程失败
        perror("pthread_join");
        return;
    }
    _isRunning = false;
}

// 线程的函数执行体
void *Thread::threadFunc(void *arg)
{
    Thread *thread = static_cast<Thread *>(arg);
    if (thread && thread->_cb) {
        thread->_cb();
    }else{
        cout << "Thread::threadFunc() error: thread is nullptr" << endl;
    }
    pthread_exit(nullptr);
}