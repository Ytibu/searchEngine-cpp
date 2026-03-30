#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__ 

#include <memory>
#include <functional>
#include <string>
#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"
// #include "EventLoop.h"  // 移除循环依赖的包含
using std::string;

// 前置声明解决循环依赖问题
class EventLoop;

/**
 * 网络连接类
 */

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection {
public:
    typedef std::function<void(const TcpConnectionPtr&)> TcpConnectionCallback;

private:
    Socket _sockfd;
    SocketIO _sockIO;
    const InetAddress _localAddr;
    const InetAddress _peerAddr;
    bool _isShutdownWrite;
    EventLoop *_loop;   //保存EventLoop对象的指针

    TcpConnectionCallback _onConnectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;

public:
    TcpConnection(int sockfd);
    ~TcpConnection();

    string receive();   // 接收数据
    void send(const string& msg);   // 发送数据
    void sendInLoop(const string& msg); //将数据交给IO线程发送
    void sendAndClose(const string& msg);   // 发送数据并关闭连接,针对网页服务
    void shutdown();    // 关闭连接

    /*设置回调函数*/
    void setConnectionCallback(TcpConnectionCallback cb);
    void setMessageCallback(TcpConnectionCallback cb);
    void setCloseCallback(TcpConnectionCallback cb);

    void handleConnectionCallback();    //调用相应的回调函数
    void handleMessageCallback();
    void handleCloseCallback();

    string toString();  //返回连接的字符串表示

    
};

#endif