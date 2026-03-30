#ifndef __PAGE_LIB_PREPROCESSOR_H__
#define __PAGE_LIB_PREPROCESSOR_H__ 

#include "Configuration.h"
#include "SplitTool.h"
#include "WebPage.h"
#include "PageLib.h"

#include <unordered_map>
#include <vector>
#include <string>

using std::unordered_map;
using std::pair;
using std::vector;


/**
 * 网页库预处理类
 */ 

class PageLibPreprocessor{
public:
    PageLibPreprocessor(Configuration &config, SplitToolCppJieba &jieba);
    void doProcess();    //预处理
private:
    void readInfoFromFile();    //根据配置信息读取网页库和位置偏移库的内容
    void cutRedundantPages();   //对冗余的网页进行去重
    void buildInvertIndexTable();   //创建倒排索引表
    void storeOnDisk();  //将经过预处理之后的网页库、位置偏移库和倒排索引表写回到磁盘上
private:
    Configuration &_conf;   // 配置对象
    SplitToolCppJieba &_jieba;    // 分词对象（修改为引用）
    vector<WebPage> _pageLib;  // 网页库容器的对象
    unordered_map<int, pair<int, int> > _offsetLib; //网页偏移库对象
    unordered_map<string, vector<pair<int, double>>> _invertIndexTable; //倒排索引表对象
};

#endif