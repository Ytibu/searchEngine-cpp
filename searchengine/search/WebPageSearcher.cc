#include "WebPageSearcher.h"
#include <string>
using std::string;

WebPageSearcher::WebPageSearcher(string keys, const TcpConnectionPtr &conn)
    : _sought(keys), _conn(conn)
{
}

// 执行查询
void WebPageSearcher::doQuery()
{
}