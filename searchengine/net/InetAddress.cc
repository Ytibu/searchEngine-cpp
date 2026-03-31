#include "InetAddress.h"

#include <string.h>

#include <iostream>

using std::cerr;
using std::endl;

InetAddress::InetAddress(unsigned short port)
{
    memset(&_addr, 0, sizeof(_addr));  // 将_addr结构体清零
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = INADDR_ANY;  // 监听所有网卡
}
InetAddress::InetAddress(const char *ip, unsigned short port)
{
    memset(&_addr, 0, sizeof(_addr));  // 将_addr结构体清零
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &_addr.sin_addr);  // 将点分十进制的ip地址转换为网络字节序
}
InetAddress::InetAddress(const struct sockaddr_in &addr)
    :_addr(addr)
{

}

// 返回点分十进制的ip地址
std::string InetAddress::ip() const
{
    char buf[INET_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET, &_addr.sin_addr, buf, sizeof(buf)) == nullptr) {
        std::cerr << "inet_ntop error" << std::endl;
        return std::string();
    }
    return std::string(buf);
}

// 返回端口号
unsigned short InetAddress::port() const
{
    return ntohs(_addr.sin_port);  // 将网络字节序的端口号转换为主机字节序
}