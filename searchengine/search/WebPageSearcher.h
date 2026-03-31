#ifndef __WEBPAGESEARCHER_H__
#define __WEBPAGESEARCHER_H__ 

#include <string>
#include "TcpConnection.h"
using std::string;

/**
 * 网页搜索类
 */

class WebPageSearcher {
public:
    WebPageSearcher(string keys, const TcpConnectionPtr &conn);
private:
    void doQuery(); // 执行查询

private:
    string _sought; // 查询关键字
    TcpConnectionPtr _conn; // TCP对象
};

#endif