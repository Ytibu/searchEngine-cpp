#include "KeyRecommander.h"

#include <set>
#include <string>
using std::string;
using std::set;

KeyRecommander::KeyRecommander(string &query, const TcpConnectionPtr &conn)
    : _queryWord(query), _conn(conn)
{

}

// 执行查询
void KeyRecommander::execute()
{

}     
// 查询索引表         
void KeyRecommander::queryIndexTable()
{

}
// 计算  
void KeyRecommander::statistics(set<int> &iset)
{

}
// 计算最小逻辑距离
int distance(const string &rhs)
{

}

// 响应客户端的请求
void response()
{

}