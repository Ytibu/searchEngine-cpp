#include "Socket.h"

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

Socket::Socket()
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
}
Socket::Socket(int sockfd)
    : _sockfd(sockfd)
{
}
Socket::~Socket()
{
    close(_sockfd);
}
void Socket::nonblock()      // 设置非阻塞模式
{
    int flags = fcntl(_sockfd, F_GETFL, 0);      // 获取当前标志
    if (flags == -1) return;
    
    flags |= O_NONBLOCK;                     // 添加非阻塞标志
    if (fcntl(_sockfd, F_SETFL, flags) == -1) return; // 应用非阻塞标志
}
void Socket::shutdownWrite() // 关闭套接字写端
{
    int ret = shutdown(_sockfd, SHUT_WR);    // 关闭写端
    if (ret == -1) return;
}
int Socket::fd() const       // 获取套接字文件描述符
{
    return _sockfd;
}