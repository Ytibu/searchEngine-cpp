#include "EventLoop.h"

#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/eventfd.h>

#include <iostream>
#include <utility>

using std::cerr;
using std::cout;
using std::endl;
using std::move;

EventLoop::EventLoop(Acceptor &acceptor)
    : _epollfd(createEpollfd())
    , _acceptor(acceptor)
    , _isLooping(false)
    , _eventList(1024) // 初始化事件列表大小
    , _eventfd(createEventFd())
{
    int listenfd = _acceptor.fd();
    addEpollReadFd(listenfd); // 将监听文件描述符添加到epoll中
}

EventLoop::~EventLoop()
{
    close(_epollfd); // 关闭epoll实例文件描述符
    close(_eventfd); // 关闭eventfd实例文件描述符
}

// 事件循环
void EventLoop::loop()
{
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
void EventLoop::doPendingFunctors() {}

void EventLoop::setConnectionCallback(EpollCallback &&cb)
{
    _onConnectionCb = move(cb);
} // 设置连接回调函数
void EventLoop::setMessageCallback(EpollCallback &&cb)
{
    _onMessageCb = move(cb);
} // 设置消息回调函数
void EventLoop::setCloseCallback(EpollCallback &&cb)
{
    _onCloseCb = move(cb);
} // 设置关闭回调函数

// 创建epollfd
int EventLoop::createEpollfd()
{
    int fd = epoll_create1(0);
    if (fd == -1)
    {
        cerr << "Failed to create epollfd: " << strerror(errno) << endl;
    }

    return fd;
}


// 创建eventFd
int EventLoop::createEventFd()
{
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd == -1)
    {
        cerr << "Failed to create eventfd: " << strerror(errno) << endl;
    }

    return fd;
}

// 将fd添加到epoll中
void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN; // 监听可读事件
    ev.data.fd = fd;     // 将fd存储在事件数据中
    if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        // 处理添加失败的情况
        cerr << "Failed to add fd to epoll: " << strerror(errno) << endl;
    }
}

// 将fd从epoll中删除
void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN; // 监听可读事件
    ev.data.fd = fd;     // 将fd存储在事件数据中
    if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &ev) == -1)
    {
        // 处理删除失败的情况
        cerr << "Failed to delete fd from epoll: " << strerror(errno) << endl;
    }
}

// 执行事件循环，由loop调用
void EventLoop::waitEpollfd()
{
    // 这里应该调用epoll_wait来等待事件发生，并将触发的事件存储在_eventList中
    // 然后根据触发的事件类型调用相应的处理函数，如handleConnection()或handleMessage()
    int readyNum = 0;
    do
    {
        readyNum = epoll_wait(_epollfd, _eventList.data(), _eventList.size(), 3000);
    } while (readyNum == -1 && errno == EINTR); // 处理被信号中断的情况

    if (readyNum == -1)
    {
        cerr << "Failed to wait on epoll: " << strerror(errno) << endl;
        return;
    }
    else if (readyNum == 0)
    {
        cout << "epoll_wait timed out" << endl;
    }
    else
    {
        if (readyNum == _eventList.size())
        {
            _eventList.resize(2 * readyNum); // 扩大事件列表大小，以便epoll_wait可以写入更多事件
        }
        for (int idx = 0; idx < readyNum; ++idx)
        {
            int fd = _eventList[idx].data.fd;
            if (fd == _acceptor.fd())
            {
                if (_eventList[idx].events & EPOLLIN)
                {
                    handleConnection(); // 处理新连接
                }
            }
            else
            {
                if (_eventList[idx].events & EPOLLIN)
                {
                    handleMessage(fd); // 处理旧连接消息
                }
            }
        }
    }
}

// 处理新连接
void EventLoop::handleConnection()
{
    int peerfd = _acceptor.accept();
    if (peerfd == -1)
    {
        cerr << "Failed to accept new connection: " << strerror(errno) << endl;
        return;
    }

    TcpConnectionPtr conn(new TcpConnection(peerfd));
    _connMap[peerfd] = conn; // 将新连接添加到连接映射中
    addEpollReadFd(peerfd);

    conn->setConnectionCallback(_onConnectionCb);
    conn->setMessageCallback(_onMessageCb);
    conn->setCloseCallback(_onCloseCb);

    conn->handleConnectionCallback(); // 处理新连接的回调
}

// 处理关闭旧连接
void EventLoop::handleMessage(int peerfd)
{
    auto iter = _connMap.find(peerfd);
    if (iter != _connMap.end())
    {
        TcpConnectionPtr conn = iter->second;
        bool flag = conn->isClosed();
        if (!flag)
        {
            conn->handleMessageCallback();
        }
        else
        {
            delEpollReadFd(peerfd);
            conn->handleCloseCallback();
            _connMap.erase(iter);
        }
    }
    else
    {
        // 连接不存在，可能已经关闭
        // 可以选择删除连接或者进行其他处理
        cout << "Connection with fd " << peerfd << " does not exist." << endl;
    }
}