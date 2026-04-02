#include "Acceptor.h"

#include <iostream>

#include <string.h>
#include <unistd.h>

#include "InetAddress.h"

Acceptor::Acceptor(const string &ip, unsigned short port)
: _listenSock()
, _addr(ip, port)
{
}

void Acceptor::ready() // 服务器监听准备
{
    setReuseAddr(true); // 地址复用
    setReusePort(true); // 端口复用
    bind();    // 绑定地址
    listen();  // 监听
}

int Acceptor::accept() // 接收新连接
{
    int connfd = ::accept(_listenSock.fd(), nullptr, nullptr);
    if (connfd < 0) {
        std::cerr << "Failed to accept new connection" << std::endl;
    }
    return connfd;
}

int Acceptor::fd() const
{
    return _listenSock.fd();
}

void Acceptor::setReuseAddr(bool on) // 地址复用
{
    int optval = on ? 1 : 0;
    if (setsockopt(_listenSock.fd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        std::cerr << "Failed to set SO_REUSEADDR" << std::endl;
    }
}
void Acceptor::setReusePort(bool on) // 端口复用
{
    int optval = on ? 1 : 0;
    if (setsockopt(_listenSock.fd(), SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
        std::cerr << "Failed to set SO_REUSEPORT" << std::endl;
    }
}
void Acceptor::bind()                // 绑定地址
{
    if (::bind(_listenSock.fd(), (struct sockaddr *)_addr.getAddr(), sizeof(struct sockaddr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
    }
}

void Acceptor::listen()              // 监听
{
    if (::listen(_listenSock.fd(), SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
    }
}