#include "WebPageQuery.h"

#include <vector>
#include <string>

using std::string;

WebPageQuery::WebPageQuery()
{
}

// 执行查询返回结果
string WebPageQuery::doQuery(const string &str)
{
}

// 加载库文件
void WebPageQuery::loadLibrary()
{
}

// 计算查询词权重
vector<double> WebPageQuery::getQueryWordsWeightVector(vector<string> &queryWords)
{
}

// 执行查询
bool WebPageQuery::executeQuery(const vector<string> &queryWords, vector<pair<int, vector<double>>> &resultVec)
{
}

// 创建json结果
string WebPageQuery::createJson(vector<int> &docIdVec, const vector<string> &queryWords)
{
}

// 返回无答案
string WebPageQuery::returnNoAnswer()
{
}