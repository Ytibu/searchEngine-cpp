#ifndef __SOCKET_H__
#define __SOCKET_H__ 

/**
 * 网络套接字类
 */
class Socket { 
public:
    Socket();
    Socket(int sockfd);
    ~Socket();
    void nonblock();    //设置非阻塞模式
    void shutdownWrite();   //关闭套接字写端
    int fd() const; //获取套接字文件描述符

private:
    int _sockfd;    //套接字文件描述符
};

#endif