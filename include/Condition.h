#ifndef __CONDITION_H__
#define __CONDITION_H__ 

#include "MutexLock.h"

class Condition { 
public:
    Condition(MutexLock& mutex);
    ~Condition();
    void wait();    // 等待
    void notify();  // 唤醒一个
    void notifyAll();   // 唤醒所有
private:
    MutexLock& _mutex;  //互斥锁
    pthread_cond_t _cond;   //条件变量
};

#endif