#include "TcpConnection.h"

#include "InetAddress.h"
#include "SocketIO.h"
#include "Socket.h"

#include <sstream>
#include <iostream>

#include <sys/socket.h>

using std::cerr;
using std::endl;
using std::ostringstream;
using std::cout;
using std::string;

TcpConnection::TcpConnection(int sockfd)
    : _sockfd(sockfd),
      _sockIO(sockfd),
      _localAddr(getLocalAddr()),
      _peerAddr(getPeerAddr()),
      _isShutdownWrite(false),
      _loop(nullptr) // 初始化EventLoop指针为nullptr
{
}
TcpConnection::~TcpConnection()
{
}

// 接收数据
string TcpConnection::recv()
{
    char buf[65535] = {0};
    _sockIO.readline(buf, sizeof(buf));
    return string(buf);
}

// 发送数据
void TcpConnection::send(const string &msg)
{
    _sockIO.writen(msg.c_str(), msg.size());
}

// 判断是否关闭连接
bool TcpConnection::isClosed() const
{
    char buf[10] = {0};
    int ret = ::recv(_sockfd.fd(), buf, sizeof(buf), MSG_PEEK);

    return ret == 0; // 如果recv返回0，表示连接已关闭
}

// 将数据交给IO线程发送
void TcpConnection::sendInLoop(const string &msg) {}

// 发送数据并关闭连接,针对网页服务
void TcpConnection::sendAndClose(const string &msg) {}

// 关闭连接
void TcpConnection::shutdown() {}

/*设置回调函数*/
void TcpConnection::setConnectionCallback(const TcpConnectionCallback &cb) { _onConnectionCb = cb; }
void TcpConnection::setMessageCallback(const TcpConnectionCallback &cb) { _onMessageCb = cb; }
void TcpConnection::setCloseCallback(const TcpConnectionCallback &cb) { _onCloseCb = cb; }

// 调用相应的回调函数
void TcpConnection::handleConnectionCallback()
{
    if (_onConnectionCb)
        _onConnectionCb(shared_from_this());
    else{
        cout << "_onConnectionCb == nullptr" << endl;
    }
}
void TcpConnection::handleMessageCallback()
{
    if (_onMessageCb)
        _onMessageCb(shared_from_this());
    else{
        cout << "_onMessageCb == nullptr" << endl;
    }
}
void TcpConnection::handleCloseCallback()
{
    if (_onCloseCb)
        _onCloseCb(shared_from_this());
    else{
        cout << "_onCloseCb == nullptr" << endl;
    }
}

//  返回连接的字符串表示
string TcpConnection::toString()
{
    ostringstream oss;
    oss
        << _localAddr.ip() << " : " << _localAddr.port()
        << " -> "
        << _peerAddr.ip() << " : " << _peerAddr.port();
    return oss.str();
}

InetAddress TcpConnection::getLocalAddr()
{
    struct sockaddr_in localAddr;
    socklen_t addrlen = sizeof(localAddr);
    if (getsockname(_sockfd.fd(), (struct sockaddr *)&localAddr, &addrlen) == -1)
    {
        cerr << "getsockname error" << endl;
    }
    return InetAddress(localAddr);
}

InetAddress TcpConnection::getPeerAddr()
{
    struct sockaddr_in peerAddr;
    socklen_t addrlen = sizeof(peerAddr);
    if (getpeername(_sockfd.fd(), (struct sockaddr *)&peerAddr, &addrlen) == -1)
    {
        cerr << "getpeername error" << endl;
    }
    return InetAddress(peerAddr);
}
