#ifndef __WEBPAGEQUERY_H__
#define __WEBPAGEQUERY_H__ 

#include "WebPage.h"
#include "SplitTool.h"
#include <map>
#include <unordered_map>
using std::unordered_map;
using std::pair;

/**
 * 网页查询类
 */
class WebPageQuery {
public:
    //WebPageSearcher();
    WebPageQuery();
    string doQuery(const string &str);  //执行查询返回结果

private:
    void loadLibrary(); //加载库文件
    vector<double> getQueryWordsWeightVector(vector<string> &queryWords);   //计算查询词权重
    bool executeQuery(const vector<string> &queryWords, vector<pair<int, vector<double>>> &resultVec);  //执行查询
    string createJson(vector<int> &docIdVec, const vector<string> &queryWords); //创建json结果
    string returnNoAnswer();    //返回无答案

private:
    SplitToolCppJieba _jieba;    //jieba分词库对象
    unordered_map<int, WebPage> _pageLib;   //网页库
    unordered_map<int, pair<int,int>> _offsetLib;   //网页偏移库
    unordered_map<string, set<pair<int,double>>> _invertIndexTable; //倒排索引表
};

#endif