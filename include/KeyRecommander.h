#ifndef __KEYRECOMMANDER_H__
#define __KEYRECOMMANDER_H__

#include "TcpConnection.h"

#include <string>
#include <queue>
#include <vector>
#include <set>
using std::string;
using std::set;
using std::priority_queue;
using std::vector;

/**
 * 关键字推荐执行类
 */
class KeyRecommander{
public:
    KeyRecommander(string &query, const TcpConnectionPtr&);
    void execute(); // 执行查询
private:
    void queryIndexTable(); // 查询索引表
    void statistics(set<int> &iset);    //计算
    int distance(const string &rhs);    //计算最小逻辑距离
    void response();    //响应客户端的请求

private:
    string _queryWord;  //查询词
    TcpConnectionPtr _conn; //与客户端进行连接的文件描述符
    priority_queue<MyResult, vector<MyResult, MyCompare>> _resultQue;   //保存候选结果集的优先队列
};

#endif