#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__ 

#include <memory>
#include <functional>

#include <string>
#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"

using std::string;
using std::function;
using std::shared_ptr;

// 前置声明解决循环依赖问题
class EventLoop;

/**
 * 网络连接类
 */


class TcpConnection : public std::enable_shared_from_this<TcpConnection> // 使TcpConnection对象能够生成shared_ptr
{
public:
    using TcpConnectionPtr = shared_ptr<TcpConnection>;
    using TcpConnectionCallback = function<void(const TcpConnectionPtr&)>;

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
    explicit TcpConnection(int sockfd, EventLoop *loop);
    ~TcpConnection();

    string recv();   // 接收数据
    void send(const string& msg);   // 发送数据
    bool isClosed() const; // 判断连接是否关闭
    void sendInLoop(const string& msg); //将数据交给IO线程发送
    void sendAndClose(const string& msg);   // 发送数据并关闭连接,针对网页服务
    void shutdown();    // 关闭连接

    /*设置回调函数*/
    void setConnectionCallback(const TcpConnectionCallback &cb);
    void setMessageCallback(const TcpConnectionCallback &cb);
    void setCloseCallback(const TcpConnectionCallback &cb);

    void handleConnectionCallback();    //调用相应的回调函数
    void handleMessageCallback();
    void handleCloseCallback();

    string toString();  //返回连接的字符串表示

private:
    // 获取本端和对端的地址信息
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();
};

#endif