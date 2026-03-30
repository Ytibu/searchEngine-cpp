#ifndef __SPLIT_TOOL_H__
#define __SPLIT_TOOL_H__ 

#include "Configuration.h"
#include "../include/cppjieba/Jieba.hpp"
#include <string>
#include <vector>
using std::string;
using std::vector;


/**
 * 抽象分词接口
 * 设置基类：SplitTool，为后续分词类提供
 * 设置继承类：SplitToolCppJieba，为后续分词提供
 */


class SplitTool
{
public:
    SplitTool();
    virtual ~SplitTool();
    virtual vector<string> cut(const string &sentence) = 0; //分词函数，纯虚函数提供接口
};

class SplitToolCppJieba: public SplitTool
{
public:
    SplitToolCppJieba(Configuration &conf);
    virtual ~SplitToolCppJieba();
    virtual vector<string> cut(const string &sentence);

private:
    Configuration & _conf;  //配置文件路径
    cppjieba::Jieba* _jieba;
};

#endif
