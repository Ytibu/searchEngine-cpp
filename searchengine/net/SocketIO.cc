#include "SocketIO.h"

SocketIO::SocketIO(int sockfd)
    : _sockfd(sockfd)
{

}
SocketIO::~SocketIO()
{
    close(_sockfd);    // 关闭socket文件描述符
}

// 读取一行数据
size_t SocketIO::readline(char *buf, size_t max)
{
    int ret = readn(buf, max - 1);  // 读取指定长度的数据，留一个字节给'\0'
    buf[ret] = '\0';  // 确保字符串以'\0'结尾
    return ret;   // 返回实际读取的字节数
}

// 读取指定长度的数据
size_t SocketIO::readn(char *buf, size_t count)
{
    // 从socket文件描述符中读取数据到buf中，读取count字节
    int ret = read(_sockfd, buf, count);  
    buf[ret] = '\0';  // 确保字符串以'\0'结尾
    return ret;   // 返回实际读取的字节数
}

// 写入数据
size_t SocketIO::writen(const char *buf, size_t count)
{
    return write(_sockfd, buf, count);  
    // 将buf中的数据写入到socket文件描述符中，写入count字节
}

// 查看内核缓冲区，获取数据
size_t SocketIO::recvPeek(char *buf, size_t count)
{
    return recv(_sockfd, buf, count, MSG_PEEK);  
    // 从socket文件描述符中读取数据到buf中，但不从内核缓冲区中移除数据，读取count字节
}