#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__ 

#include "Socket.h"
#include "InetAddress.h"

/**
 * 接收器类
 */


class Acceptor { 
public:
    Acceptor(const string &ip, const unsigned short port);
    void ready();   //服务器监听准备
    int accept();   //接收新连接
    int fd() const;
private:
    void setReuseAddr(bool on = true); //地址复用
    void setReusePort(bool on = true); //端口复用
    void bind();    //绑定地址
    void listen();  //监听
private:
    Socket _listenSock; //服务器监听的socket对象
    InetAddress _addr;  //服务器监听的ip和端口
};
#endif