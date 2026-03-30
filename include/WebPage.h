#ifndef __WEBPAGE_H__
#define __WEBPAGE_H__ 

#include "Configuration.h"
#include "SplitTool.h"
#include <string>
#include <vector>
#include <map>
#include <cstdint>

using std::string;
using std::vector;
using std::map;
using std::pair;

/**
 * 网页类
 */

const static int TOPK_NUMBER = 20;
const static int MAX = 8;


class WebPage {
public:
    WebPage(string &doc, Configuration &config, SplitToolCppJieba &jieba);
    int getDocId()const; //获取文档id
    string getDoc() const;//获取文档
    map<string,int> &getWordsMap(); //文档词频统计map
    //uint64_t getSimHash() const;    //获取文档的simhash值

private:
    void processDoc(const string &doc, Configuration &config, SplitToolCppJieba &jieba);  //对文档进行格式化处理
    void calcTopK(vector<string> &keywords, int k, set<string> &stopWordList);   //获取文档的topk词集
    //void setSimHash(Configuration &config,const string &docString);
    //size_t getByteNum_Utf8(const char byte);
    
friend bool operator==(const WebPage & lhs, const WebPage & rhs);   //判断两篇文档是否相等
friend bool operator<(const WebPage & lhs, const WebPage & rhs); //对文档DocId进行排序

private:
    string _doc;//整篇文章，包括xml
    size_t _docId; //文档id
    string _docTitle;   //文档标题
    string _docUrl; //文档url
    string _docContent; //文档内容
    string _docSummary; //文档摘要，自动生成，不固定
    vector<string> _topWords;   //词频最高的前20个词
    map<string,int> _wordsMap;  //保存每篇文档的所有词语和词频，不包含停用词
    //uint64_t _simHash;

};

#endif