#ifndef __SEARCH_ENGINE_SERVER_H__
#define __SEARCH_ENGINE_SERVER_H__ 

#include "../net/TcpConnection.h"
#include "../net/TcpServer.h"
#include "../net/ThreadPool.h"
#include <string>
using std::string;

/**
 * 查询服务器类
 */
class SearchEngineServer {
public:
    SearchEngineServer(const string &ip,const unsigned short port, const int threadNum, const int taskSize);
    void start();   // 启动服务器

private:
    void onConnect(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn);
    void onClose(const TcpConnectionPtr &conn);
    void doTaskThread(const TcpConnectionPtr &conn, const string &msg);
private:
    TcpServer _tcpServer;   // TCP通信模块
    ThreadPool _pool;   // 线程池
};

#endif