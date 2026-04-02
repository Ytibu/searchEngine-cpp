#include "SocketIO.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>

using std::cerr;
using std::endl;

SocketIO::SocketIO(int sockfd)
    : _sockfd(sockfd)
{
}
SocketIO::~SocketIO()
{
    // SocketIO仅负责收发，不拥有fd生命周期，避免和Socket重复close。
}

// 读取一行数据
size_t SocketIO::readline(char *buf, size_t max)
{
    int left = max - 1; // 剩余要读取的字节数，预留一个字节给字符串结束符
    char *ptr = buf;    // 指向buf的指针
    int ret = 0, total = 0;        // 实际读取的字节数
    while (left > 0)
    {
        ret = recv(_sockfd, ptr, left, MSG_PEEK); // 从socket文件描述符中读取数据到ptr中，但不从内核缓冲区中移除数据，读取left字节
        if (ret == -1)
        { // 读取出错
            if (errno == EINTR)
            { // 被信号中断，继续读取
                continue;
            }
            cerr << "readline error" << endl; // 输出错误信息
            return 0;                         // 其他错误，返回0表示读取失败
        }
        else if (ret == 0)
        {          // 对方关闭连接
            break; // 退出循环
        }

        for (int idx = 0; idx < ret; ++idx)
        {
            if (ptr[idx] == '\n')
            {
                int sz = idx + 1; // 包括换行符在内的实际读取字节数
                readn(ptr, sz);   // 从socket文件描述符中读取数据到ptr中
                ptr += sz;
                *ptr = '\0'; // 添加字符串结束符

                return total + sz;
            }
        }
        readn(ptr, ret); // 从socket文件描述符中读取数据到ptr中
        total += ret;
        ptr += ret;
        left -= ret;
    }

    *ptr = '\0'; // 添加字符串结束符
    return total; // 返回实际读取的字节数
}

// 读取指定长度的数据
size_t SocketIO::readn(char *buf, size_t count)
{
    // 从socket文件描述符中读取数据到buf中，读取count字节
    int left = count; // 剩余要读取的字节数
    char *ptr = buf;  // 指向buf的指针
    int ret = 0;      // 实际读取的字节数
    while (left > 0)
    {
        ret = read(_sockfd, ptr, left);
        if (ret == -1)
        { // 读取出错
            if (errno == EINTR)
            { // 被信号中断，继续读取
                continue;
            }
            cerr << "read error" << endl; // 输出错误信息
            return -1;                    // 其他错误，返回-1
        }
        else if (ret == 0)
        {          // 对方关闭连接
            break; // 退出循环
        }
        left -= ret; // 更新剩余要读取的字节数
        ptr += ret;  // 移动指针
    }
    return count - left; // 返回实际读取的字节数
}

// 写入数据
size_t SocketIO::writen(const char *buf, size_t count)
{
    int left = count;      // 剩余要写入的字节数
    const char *ptr = buf; // 指向buf的指针
    int ret = 0;           // 实际写入的字节数
    while (left > 0)
    {
        ret = write(_sockfd, ptr, left);
        if (ret == -1)
        { // 写入出错
            if (errno == EINTR)
            { // 被信号中断，继续写入
                continue;
            }
            cerr << "write error" << endl; // 输出错误信息
            return -1;                     // 其他错误，返回-1
        }
        else if (ret == 0)
        {          // 对方关闭连接
            break; // 退出循环
        }
        left -= ret; // 更新剩余要写入的字节数
        ptr += ret;  // 移动指针
    }
    return count - left; // 返回实际写入的字节数
}

// 查看内核缓冲区，获取数据
size_t SocketIO::recvPeek(char *buf, size_t count)
{
    return recv(_sockfd, buf, count, MSG_PEEK);
    // 从socket文件描述符中读取数据到buf中，但不从内核缓冲区中移除数据，读取count字节
}