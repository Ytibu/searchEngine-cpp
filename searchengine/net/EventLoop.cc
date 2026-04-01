#include "EventLoop.h"

EventLoop::EventLoop(Acceptor &acceptor)
    : _epollfd()
    , _eventfd()
    , _acceptor(acceptor)
    , _isLooping(false)
    , _eventList(1024) // 初始化事件列表大小
{
}

EventLoop::~EventLoop()
{

}

// 事件循环
void EventLoop::loop()
{
    if(_isLooping)
    {
        return; // 已经在循环中，直接返回
    }
    _isLooping = true;
    while (_isLooping)
    {
        waitEpollfd(); // 等待事件发生
    }
}

// 退出事件循环
void EventLoop::unloop()
{
    _isLooping = false;
}

// 向IO线程发送数据
void EventLoop::runInLoop(const Functor &cb)
{
    if (cb)
    {
        _pendingFunctors.push_back(cb);
    }
}

// 激活_eventfd(执行写操作)
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(_eventfd, &one, sizeof(one));
    if (n != sizeof(one))
    {
        // 处理写入错误
    }
}

// 处理_eventfd(执行写操作)
void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(_eventfd, &one, sizeof(one));
    if (n != sizeof(one))
    {
        // 处理读取错误
    }
}

// 执行待处理的回调函数
void EventLoop::doPendingFunctors(){}

void EventLoop::setConnectionCallback(EpollCallback cb){} // 设置连接回调函数
void EventLoop::setMessageCallback(EpollCallback cb){}    // 设置消息回调函数
void EventLoop::setCloseCallback(EpollCallback cb){}      // 设置关闭回调函数


// 执行事件循环，由loop调用
void EventLoop::waitEpollfd(){}

// 处理新连接
void EventLoop::handleNewConnection()
{}

// 处理关闭旧连接
void EventLoop::handleMessage(int peerfd)
{
    auto iter = _connMap.find(peerfd);
    if (iter != _connMap.end())
    {
        TcpConnectionPtr conn = iter->second;
        conn->handleMessageCallback();
    }
}