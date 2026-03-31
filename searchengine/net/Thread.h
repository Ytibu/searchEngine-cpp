#ifndef __THREAD_H__
#define __THREAD_H__ 

#include <unixHeader.h>
#include <functional>

class Thread { 
public:
    typedef std::function<void()> ThreadCallback;   //重定义回调函数的标签

public:
    Thread(ThreadCallback cb);  //构造函数
    ~Thread();
    void start();   //启动线程
    void join();    //等待线程结束

private:
    static void * threadFunc(void * arg);   //线程的函数执行体

private:
    pthread_t _pthId;   //线程id
    bool _isRunning;    //线程运行状态
    ThreadCallback _cb; //执行任务的函数对象


};

#endif