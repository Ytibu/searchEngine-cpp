#include "Acceptor.h"

#include <iostream>

#include <string.h>
#include <unistd.h>

#include "InetAddress.h"

Acceptor::Acceptor(int fd, const InetAddress &addr)
    : _listenSock(fd), _addr(addr) {
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
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_addr.port());
    inet_pton(AF_INET, _addr.ip().c_str(), &addr.sin_addr);
    if (::bind(_listenSock.fd(), (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
    }
}

void Acceptor::listen()              // 监听
{
    if (::listen(_listenSock.fd(), SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
    }
}