#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__ 

#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"

/**
 * Tcp服务器类
 */

class TcpServer {
public:
    typedef TcpConnection::TcpConnectionCallback TcpServerCallback;

public:
    TcpServer(unsigned short port);
    TcpServer(const char *ip, unsigned short port);
    void start();   // 启动服务器
    void stop();    // 停止服务器
    void setConnectionCallback(TcpServerCallback cb);
    void setMessageCallback(TcpServerCallback cb);
    void setCloseCallback(TcpServerCallback cb);
    
private:
   Acceptor _acceptor; // 接受器对象
    EventLoop _eventLoop; // epoll对象

};

#endif