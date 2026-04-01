#include "TcpServer.h"

TcpServer::TcpServer(unsigned short port)
{
}
TcpServer::TcpServer(const char *ip, unsigned short port)
{

}
void TcpServer::start() // 启动服务器
{
}
void TcpServer::stop()  // 停止服务器
{
}
void TcpServer::setConnectionCallback(TcpServerCallback cb)
{
}
void TcpServer::setMessageCallback(TcpServerCallback cb)
{
}
void TcpServer::setCloseCallback(TcpServerCallback cb)
{
}