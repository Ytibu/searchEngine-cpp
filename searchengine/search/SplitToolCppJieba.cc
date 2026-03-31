#include "SplitTool.h"
#include "../../include/cppjieba/Jieba.hpp"
#include <iostream>
using std::cout;
using std::map;

// Configuration &conf
SplitToolCppJieba::SplitToolCppJieba(Configuration &conf)
    : _conf(conf)
{
    auto map = _conf.getConfigMap();

    // 修复：使用std::string保存配置路径，而不是直接使用c_str()指针
    std::string DICT_PATH = map["DICT_PATH"];
    std::string HMM_PATH = map["HMM_PATH"];
    std::string USER_DICT_PATH = map["USER_DICT_PATH"];
    std::string IDF_PATH = map["IDF_PATH"];
    std::string STOP_WORD_PATH = map["STOP_WORD_PATH"];

    // 初始化Jieba分词器，只初始化一次
    _jieba = new cppjieba::Jieba(DICT_PATH.c_str(), HMM_PATH.c_str(),
                                 USER_DICT_PATH.c_str(), IDF_PATH.c_str(),
                                 STOP_WORD_PATH.c_str());
}

SplitToolCppJieba::~SplitToolCppJieba()
{
    delete _jieba;
}

vector<string> SplitToolCppJieba::cut(const string &sentence)
{
    vector<string> result;
    _jieba->Cut(sentence, result, true);
    return result;
}