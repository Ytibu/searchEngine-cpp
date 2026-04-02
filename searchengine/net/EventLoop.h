#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "Acceptor.h"
#include "TcpConnection.h"
#include "MutexLock.h"

#include <vector>
#include <map>
#include <functional>

/**
 * epoll封装类
 */

using EventList = std::vector<struct epoll_event>;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionMap = std::map<int, TcpConnectionPtr>;

using Functor = std::function<void()>; // 函数回调重定义
using EpollCallback = TcpConnection::TcpConnectionCallback;

class EventLoop
{
private:
    int _epollfd; // epoll实例文件描述符

    Acceptor &_acceptor; // 服务武器监听文件描述符
    bool _isLooping;     // 标记是否进行循环

    EventList _eventList;   // 存储触发事件的fd
    ConnectionMap _connMap; // 存储已经建立的连接

    EpollCallback _onConnectionCb;
    EpollCallback _onMessageCb;
    EpollCallback _onCloseCb;

    int _eventfd; // eventfd实例文件描述符,线程间通信
    std::vector<Functor> _pendingFunctors; // 需要延迟执行的回调函数
    MutexLockGuard _mutexLockGuard;        // 保护_pendingFunctors的互斥锁

public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();

    void loop();   // 事件循环
    void unloop(); // 退出事件循环

private:
    int createEpollfd();         // 创建epollfd
    void addEpollReadFd(int fd); // 将fd添加到epoll中
    void delEpollReadFd(int fd); // 将fd从epoll中删除
    void waitEpollfd();          // 执行事件循环，由loop调用

    void handleConnection();        // 处理新连接
    void handleMessage(int peerfd); // 处理连接消息

private:
    int createEventFd();      // 创建eventfd
    void handleRead();        // 处理_eventfd(执行写操作)
    void doPendingFunctors(); // 执行待处理的回调函数
public:
    void runInLoop(const Functor &cb); // 向IO线程发送数据
    void wakeup();                     // 激活_eventfd(执行写操作)
public:
    void setConnectionCallback(EpollCallback &&cb); // 设置连接回调函数
    void setMessageCallback(EpollCallback &&cb);    // 设置消息回调函数
    void setCloseCallback(EpollCallback &&cb);      // 设置关闭回调函数
};

#endif