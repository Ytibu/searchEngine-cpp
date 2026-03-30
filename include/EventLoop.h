#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__ 

#include "Acceptor.h"
#include "TcpConnection.h"
#include <vector>
#include <map>
#include <functional>


/**
 * epoll封装类
 */

class EventLoop {

public:
    typedef std::function<void()> Functor;  //函数回调重定义
    typedef TcpConnection::TcpConnectionCallback EpollCallback;

private:
    int _epollfd;   // epoll实例文件描述符
    int _eventfd;   // eventfd实例文件描述符,线程间通信

    Acceptor &_acceptor;    //服务武器监听文件描述符
    bool _isLooping;    // 标记是否进行循环
    
    typedef std::vector<struct epoll_event> EventList;
    EventList _eventList;   //存储触发事件的fd
    typedef std::map<int, TcpConnectionPtr> ConnectionMap;
    ConnectionMap _connMap; //存储已经建立的连接

    EpollCallback _onConnectionCb;
    EpollCallback _onMessageCb;
    EpollCallback _onCloseCb;

    std::vector<Functor> _pendingFunctors;  //需要延迟执行的回调函数

public:
    EventLoop(Acceptor & acceptor);  // 构造函数参数类型修正
    ~EventLoop();

    void loop();    //事件循环
    void unloop();  //退出事件循环

    void runInLoop(const Functor& cb);  //向IO线程发送数据

    void wakeup();  //激活_eventfd(执行写操作)
    void handleRead();  //处理_eventfd(执行写操作)
private:
    void doPendingFunctors();   //执行待处理的回调函数

    void setConnectionCallback(EpollCallback cb);   //设置连接回调函数
    void setMessageCallback(EpollCallback cb);  //设置消息回调函数
    void setCloseCallback(EpollCallback cb);    //设置关闭回调函数
private:
    void waitEpollfd(); //执行事件循环，由loop调用
    void handleNewConnection(); //处理新连接
    void handleMessage(int peerfd);   //处理旧连接消息

};

#endif