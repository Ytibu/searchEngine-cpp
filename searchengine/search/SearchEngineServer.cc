#include "SearchEngineServer.h"
#include "../net/TcpServer.h"
#include "../net/ThreadPool.h"

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

SearchEngineServer::SearchEngineServer(const string &ip, const unsigned short port, const int threadNum, const int taskSize)
    : _tcpServer(ip, port), _pool(threadNum, taskSize)
{
}

// 启动服务器
void SearchEngineServer::start()
{
    using namespace std::placeholders;

    // 注册回调函数到底层 TcpServer
    _tcpServer.setConnectionCallback(std::bind(&SearchEngineServer::onConnect, this, _1));
    _tcpServer.setMessageCallback(std::bind(&SearchEngineServer::onMessage, this, _1));
    _tcpServer.setCloseCallback(std::bind(&SearchEngineServer::onClose, this, _1));

    _pool.start();
    _tcpServer.start();
}
void SearchEngineServer::onConnect(const TcpConnectionPtr &conn)
{
    cout << "New connection: " << conn->toString() << endl;
}
void SearchEngineServer::onMessage(const TcpConnectionPtr &conn)
{
    string msg = conn->recv();
    cout << "Received message: " << msg << endl;

    doTaskThread(conn, msg);
}
void SearchEngineServer::onClose(const TcpConnectionPtr &conn)
{
    cout << "Connection closed: " << conn->toString() << endl;
}
void SearchEngineServer::doTaskThread(const TcpConnectionPtr &conn, const string &msg)
{
    // 这里可以根据 msg 的内容来决定具体的任务类型，并将任务添加到线程池中
    _pool.addTask([conn, msg]()
                  {
                      // 处理任务的具体逻辑，例如查询数据库、计算结果等
                      string result = "Processed: " + msg; // 这是一个示例结果
                      conn->sendInLoop(result);            // 将结果发送回客户端
                  });
}