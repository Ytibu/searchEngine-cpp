#include "TcpConnection.h"

#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketIO.h"
#include "Socket.h"

#include <sys/socket.h>

#include <sstream>
#include <iostream>
#include <cctype>

using std::cerr;
using std::cout;
using std::endl;
using std::ostringstream;
using std::string;

TcpConnection::TcpConnection(int sockfd, EventLoop *loop)
    : _sockfd(sockfd),
      _sockIO(sockfd),
      _loop(loop),
      _localAddr(getLocalAddr()),
      _peerAddr(getPeerAddr()),
      _isShutdownWrite(false)
{
}
TcpConnection::~TcpConnection()
{
}

// 接收数据
string TcpConnection::recv()
{
    auto trim = [](std::string &s) {
        while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || std::isspace(static_cast<unsigned char>(s.back())))) {
            s.pop_back();
        }
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
            ++start;
        }
        if (start > 0) {
            s.erase(0, start);
        }
    };

    std::string request;
    char buf[65535] = {0};

    // 1) 先读取请求头，直到 CRLF CRLF
    while (true)
    {
        size_t n = _sockIO.readline(buf, sizeof(buf));
        if (n == 0)
        {
            break;
        }

        request.append(buf, n);

        if (request.size() >= 4 && request.compare(request.size() - 4, 4, "\r\n\r\n") == 0)
        {
            break;
        }
    }

    if (request.empty())
    {
        return request;
    }

    // 2) 解析 Content-Length 并补齐 body
    size_t contentLength = 0;
    std::istringstream headerStream(request);
    std::string line;
    while (std::getline(headerStream, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if (line.empty())
        {
            break;
        }

        const size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        trim(key);
        trim(value);

        if (key == "Content-Length" || key == "content-length")
        {
            try
            {
                contentLength = static_cast<size_t>(std::stoul(value));
            }
            catch (...)
            {
                contentLength = 0;
            }
        }
    }

    if (contentLength > 0)
    {
        std::string body(contentLength, '\0');
        const size_t readBytes = _sockIO.readn(&body[0], contentLength);
        if (readBytes != contentLength)
        {
            cerr << "TcpConnection::recv body truncated: expected "
                 << contentLength << " bytes, got " << readBytes << endl;
            return std::string();
        }
        request += body;
    }

    return request;
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
void TcpConnection::sendInLoop(const string &msg)
{
    if (_loop)
    {
        _loop->runInLoop(bind(&TcpConnection::send, this, msg));
    }
}

// 发送数据并关闭连接,针对网页服务
void TcpConnection::sendAndClose(const string &msg)
{
    send(msg);
    shutdown();
}

// 关闭连接
void TcpConnection::shutdown()
{
    if (!_isShutdownWrite)
    {
        _isShutdownWrite = true;
        _sockfd.shutdownWrite();
    }
}

/*设置回调函数*/
void TcpConnection::setConnectionCallback(const TcpConnectionCallback &cb) { _onConnectionCb = cb; }
void TcpConnection::setMessageCallback(const TcpConnectionCallback &cb) { _onMessageCb = cb; }
void TcpConnection::setCloseCallback(const TcpConnectionCallback &cb) { _onCloseCb = cb; }

// 调用相应的回调函数
void TcpConnection::handleConnectionCallback()
{
    if (_onConnectionCb)
        _onConnectionCb(shared_from_this());
    else
    {
        cout << "_onConnectionCb == nullptr" << endl;
    }
}
void TcpConnection::handleMessageCallback()
{
    if (_onMessageCb)
        _onMessageCb(shared_from_this());
    else
    {
        cout << "_onMessageCb == nullptr" << endl;
    }
}
void TcpConnection::handleCloseCallback()
{
    if (_onCloseCb)
        _onCloseCb(shared_from_this());
    else
    {
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
    if (::getsockname(_sockfd.fd(), (struct sockaddr *)&localAddr, &addrlen) == -1)
    {
        cerr << "getsockname error" << endl;
    }
    return InetAddress(localAddr);
}

InetAddress TcpConnection::getPeerAddr()
{
    struct sockaddr_in peerAddr;
    socklen_t addrlen = sizeof(peerAddr);
    if (::getpeername(_sockfd.fd(), (struct sockaddr *)&peerAddr, &addrlen) == -1)
    {
        cerr << "getpeername error" << endl;
    }
    return InetAddress(peerAddr);
}
