#include "MutexLock.h"

#include <pthread.h>
#include <stdio.h>

MutexLock::MutexLock()
{
    int ret = pthread_mutex_init(&_mutex, nullptr); // 初始化互斥锁
    if (ret)
    {
        perror("pthread_mutex_init error");
    }
}

MutexLock::~MutexLock()
{
    int ret = pthread_mutex_destroy(&_mutex); // 销毁互斥锁
    if (ret)
    {
        perror("pthread_mutex_destroy error");
    }
}

void MutexLock::lock()
{
    int ret = pthread_mutex_lock(&_mutex); // 加锁
    if (ret)
    {
        perror("pthread_mutex_lock error");
    }
}

bool MutexLock::tryLock()
{
    int ret = pthread_mutex_trylock(&_mutex); // 尝试加锁
    if (ret)
    {
        perror("pthread_mutex_trylock error");
        return false; // 加锁失败
    }

    return ret == 0; // 返回是否成功加锁
}

void MutexLock::unlock()
{
    int ret = pthread_mutex_unlock(&_mutex); // 解锁
    if (ret)
    {
        perror("pthread_mutex_unlock error");
    }
}

// 获取原生互斥锁的地址，由Condition对象调用
pthread_mutex_t *MutexLock::getMutexPtr()
{
    return &_mutex; // 返回互斥锁的地址
}


// MutexLockGuard类的实现:利用RAII机制自动加锁和解锁
MutexLockGuard::MutexLockGuard(MutexLock &mutex) : _mutex(mutex)
{
    _mutex.lock(); // 构造函数加锁
}

MutexLockGuard::~MutexLockGuard()
{
    _mutex.unlock(); // 析构函数解锁
}