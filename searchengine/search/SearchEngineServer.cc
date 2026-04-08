#include "SearchEngineServer.h"
#include "../net/TcpServer.h"
#include "../net/ThreadPool.h"
#include "HttpParser.h"

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
    cout << "-------------------------------------------" << endl;
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
    cout << "-------------------------------------------" << endl;
}

void SearchEngineServer::doTaskThread(const TcpConnectionPtr &conn, const string &msg)
{
    // 这里可以根据 msg 的内容来决定具体的任务类型，并将任务添加到线程池中
    _pool.addTask([conn, msg](){
        // 2. 解析 HTTP 请求
        HttpParser::HttpRequest req = HttpParser::parseRequest(msg);
        if(req.path == "/api/v1/keywords/recommendations" && req.method == HttpParser::HttpMethod::POST) {
            // 3. 业务逻辑 (假设获取到了推荐词的 json 字符串)
            HttpParser parser;
            std::string resultJson = parser.getRecommendations(req.body);

            if (resultJson.empty()) {
                std::string errResp = HttpParser::buildErrorResponse(4001, "intent can not be empty");
                conn->sendAndClose(errResp);
                return;
            }

            // 4. 组装响应
            std::string httpResp = HttpParser::buildSuccessResponse(resultJson);
            cout << "Response: " << httpResp << endl;
        
            // 5. 将完整的 HTTP 报文发送给客户端
            conn->sendAndClose(httpResp);            // 将结果发送回客户端并关闭连接
        }else if(req.path == "/api/v1/search/results" && req.method == HttpParser::HttpMethod::POST){
            // 3. 业务逻辑 (假设获取到了搜索结果的 json 字符串)
            HttpParser parser;
            std::string resultJson = parser.getResults(req.body); // 这里暂时复用 getResults 来模拟搜索结果

            if (resultJson.empty()) {
                std::string errResp = HttpParser::buildErrorResponse(4002, "query can not be empty");
                conn->sendAndClose(errResp);
                return;
            }

            // 4. 组装响应
            std::string httpResp = HttpParser::buildSuccessResponse(resultJson);
            cout << "Response: " << httpResp << endl;
        
            // 5. 将完整的 HTTP 报文发送给客户端
            conn->sendAndClose(httpResp);            // 将结果发送回客户端并关闭连接
        } else {
            std::string errResp = HttpParser::buildErrorResponse(404, "Not Found");
            conn->sendAndClose(errResp);
        } 
    });
}