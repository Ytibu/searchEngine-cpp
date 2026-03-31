#include "Condition.h"

#include "MutexLock.h"

#include <iostream>

#include <stdio.h>
#include <string.h>
#include <pthread.h>

using std::cerr;
using std::endl;

Condition::Condition(MutexLock &mutex)
    : _mutex(mutex)
{
    int ret = pthread_cond_init(&_cond, nullptr);
    if (ret)
    {
        cerr << "pthread_cond_init error: " << strerror(ret) << endl;
        return;
    }
}
Condition::~Condition()
{
    int ret = pthread_cond_destroy(&_cond);
    if (ret)
    {
        cerr << "pthread_cond_destroy error: " << strerror(ret) << endl;
        return;
    }
}
void Condition::wait() // 等待
{
    int ret = pthread_cond_wait(&_cond, _mutex.getMutexPtr());
    if (ret)
    {
        cerr << "pthread_cond_wait error: " << strerror(ret) << endl;
        return;
    }
}
void Condition::notify() // 唤醒一个
{
    int ret = pthread_cond_signal(&_cond);
    if (ret)
    {
        cerr << "pthread_cond_signal error: " << strerror(ret) << endl;
        return;
    }
}
void Condition::notifyAll() // 唤醒所有
{
    int ret = pthread_cond_broadcast(&_cond);
    if (ret)
    {
        cerr << "pthread_cond_broadcast error: " << strerror(ret) << endl;
        return;
    }
}