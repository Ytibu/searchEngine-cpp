#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__ 

//#include <unixHeader.h>
#include <string>
#include <arpa/inet.h>

/**
 * 网络地址类
*/

class InetAddress {
public:
    InetAddress(unsigned short port);
    InetAddress(const char *ip, unsigned short port);
    InetAddress(const struct sockaddr_in &addr);
    std::string ip() const;  // 返回点分十进制的ip地址
    unsigned short port() const;    // 返回端口号
    
private:
    struct sockaddr_in _addr;   // sockaddr_in结构体
};

#endif