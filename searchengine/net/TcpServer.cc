#include "TcpServer.h"

#include <sys/socket.h>

#include <iostream>
#include <utility>

#include "InetAddress.h"

TcpServer::TcpServer(const char *ip, unsigned short port)
    :_acceptor(ip, port), _eventLoop(_acceptor)
{
}

void TcpServer::start() // 启动服务器
{
	_acceptor.ready();
	_eventLoop.loop();
}

void TcpServer::stop()  // 停止服务器
{
	_eventLoop.unloop();
}

void TcpServer::setConnectionCallback(TcpServerCallback &&cb)
{
	_eventLoop.setConnectionCallback(std::move(cb));
}

void TcpServer::setMessageCallback(TcpServerCallback &&cb)
{
	_eventLoop.setMessageCallback(std::move(cb));
}

void TcpServer::setCloseCallback(TcpServerCallback &&cb)
{
	_eventLoop.setCloseCallback(std::move(cb));
}