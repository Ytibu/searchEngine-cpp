#include "TcpConnection.h"

TcpConnection::TcpConnection(int sockfd)
{
}
TcpConnection::~TcpConnection()
{
}

// 接收数据
string TcpConnection::receive()
{
}

// 发送数据
void TcpConnection::send(const string &msg)
{
}

// 将数据交给IO线程发送
void TcpConnection::sendInLoop(const string &msg)
{
}

// 发送数据并关闭连接,针对网页服务
void TcpConnection::sendAndClose(const string &msg)
{
}

// 关闭连接
void TcpConnection::shutdown()
{
}

/*设置回调函数*/
void TcpConnection::setConnectionCallback(TcpConnectionCallback cb)
{
}
void TcpConnection::setMessageCallback(TcpConnectionCallback cb)
{
}
void TcpConnection::setCloseCallback(TcpConnectionCallback cb) {}

// 调用相应的回调函数
void TcpConnection::handleConnectionCallback() {}
void TcpConnection::handleMessageCallback() {}
void TcpConnection::handleCloseCallback() {}

//  返回连接的字符串表示
string TcpConnection::toString()
{
}