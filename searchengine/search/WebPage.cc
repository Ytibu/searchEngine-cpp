#include "WebPage.h"
#include "../base/Configuration.h"
#include "SplitTool.h"
#include "DirScanner.h"

#include "../../include/tinyxml/tinyxml2.h"
#include "../../include/simhash/Simhasher.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cstdint>

using std::cerr;
using std::cout;
using std::ifstream;
using std::regex;
using std::regex_replace;
using std::string;

WebPage::WebPage(string &doc, Configuration &config, SplitToolCppJieba &jieba)
    : _doc(doc)
{
    // 处理文档，每个文档转换成一个网页对象
    processDoc(doc, config, jieba);
}
// 获取文档id
int WebPage::getDocId() const
{
    return _docId;
}

// 获取文档
string WebPage::getDoc() const
{
    string fmtTXT = string("<doc>\n") +
                    "<docid>" + std::to_string(_docId) + "</docid>\n" +
                    "<url>" + _docUrl + "</url>\n" +
                    "<title>" + _docTitle + "</title>\n" +
                    "<content>" + _docContent + "</content>\n</doc>\n";
    return fmtTXT;
}

// 文档词频统计map
map<string, int> &WebPage::getWordsMap()
{
    return _wordsMap;
}

// 对文档进行格式化处理，并生成一个文档对象，文章内容交给topK生成词频和topK
void WebPage::processDoc(const string &docString, Configuration &config, SplitToolCppJieba &jieba)
{
    // 获取停用词
    set<string> stopWordList = config.getStopWordList();

    tinyxml2::XMLDocument root;
    root.Parse(docString.c_str());
    if (root.Error())
    {
        perror("WebPage::processDoc: tinyxml2 parse error");
    }
    tinyxml2::XMLElement *doc = root.FirstChildElement("doc");
    if (doc)
    {
        const char *docIdText = doc->FirstChildElement("docid")->GetText();
        const char *urlText = doc->FirstChildElement("url")->GetText();
        const char *titleText = doc->FirstChildElement("title")->GetText();
        const char *contentText = doc->FirstChildElement("content")->GetText();

        _docId = docIdText ? atoi(docIdText) : 0;
        _docUrl = urlText ? urlText : "";
        _docTitle = titleText ? titleText : "";
        _docContent = contentText ? contentText : "";
    }

    vector<string> cutWords = jieba.cut(_docContent);
    calcTopK(cutWords, TOPK_NUMBER, stopWordList);
}

// 获取所有词频和topK
void WebPage::calcTopK(vector<string> &wordsVec, int k, set<string> &stopWordList)
{
    // 清空容器
    _wordsMap.clear();

    // 如果不在停用词列表中，自动加入_wordsMap中
    for (auto &keyword : wordsVec)
    {
        if (stopWordList.find(keyword) == stopWordList.end())
        {
            _wordsMap[keyword]++;
        }
    }

    // 创建词频向量并排序
    vector<pair<string, int>> wordFreqVec(_wordsMap.begin(), _wordsMap.end());
    sort(wordFreqVec.begin(), wordFreqVec.end(), [](const pair<string, int> &a, const pair<string, int> &b)
         {
             return a.second > b.second; // 按词频降序排列
         });

    _topWords.clear();

    // 取前k个高频词
    int count = 0;
    for (auto &pair : wordFreqVec)
    {
        if (count >= k)
            break;
        _topWords.push_back(pair.first);
        count++;
    }
}

// 判断两篇文档是否相等
bool operator==(const WebPage &lhs, const WebPage &rhs)
{
    int count = 0;
    for (auto &keyword : lhs._topWords)
    {
        if (!(rhs._wordsMap.find(keyword) == rhs._wordsMap.end()))
        {
            count++;
        }
    }

    return count >= MAX;
}

// 对文档DocId进行排序
bool operator<(const WebPage &lhs, const WebPage &rhs)
{
    return lhs._docId < rhs._docId;
}