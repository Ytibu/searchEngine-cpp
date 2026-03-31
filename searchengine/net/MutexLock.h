#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

#include <sys/types.h>

#include "../base/Noncopyable.h"

class MutexLock : NonCopyable
{
public:
    MutexLock();
    ~MutexLock();
    void lock();
    bool tryLock();
    void unlock();
    pthread_mutex_t *getMutexPtr(); // 获取原生互斥锁的地址，由Condition对象调用
private:
    pthread_mutex_t _mutex; // 互斥变量
};

class MutexLockGuard : NonCopyable
{
public:
    MutexLockGuard(MutexLock &mutex);
    ~MutexLockGuard();
private:    
    MutexLock &_mutex; // 引用类型的互斥锁对象
};
#endif