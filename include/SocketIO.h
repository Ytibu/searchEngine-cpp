#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__ 

#include <Cfun.h>

/**
 * 网络IO类
 */

class SocketIO {
public:
    SocketIO(int sockfd);
    ~SocketIO();
    size_t readline(char *buf, size_t max); // 读取一行数据
    size_t readn(char *buf, size_t count);  // 读取指定长度的数据
    size_t writen(const char *buf, size_t count);   // 写入数据
    size_t recvPeek(char *buf, size_t count);   //查看内核缓冲区，获取数据

private:
    int _sockfd;    // socket文件描述符
};

#endif