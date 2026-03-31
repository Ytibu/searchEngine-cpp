#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__ 

#include <unixHeader.h>

class MutexLock{
public:
    MutexLock();
    ~MutexLock();
    void lock();
    void unlock();
    pthread_mutex_t * getMutexPtr();    //获取原生互斥锁的地址，由Condition对象调用
private:
    pthread_mutex_t _mutex; //互斥变量
};

#endif