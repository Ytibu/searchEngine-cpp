#include "Configuration.h"
#include "../include/tinyxml/tinyxml2.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::string;
using std::vector;

Configuration &Configuration::getInstance(const string &filePath)
{
    static Configuration instance(filePath);
    return instance;
}

// 导入文件路径，构造
Configuration::Configuration(const string &filePath)
    : _filePath(filePath)
{
}

Configuration::~Configuration()
{
}

// 获取对应配置项内容
map<string, string> &Configuration::getConfigMap()
{
    // 如果已经加载过配置，直接返回
    if (!_configMap.empty())
    {
        return _configMap;
    }

    tinyxml2::XMLDocument doc;
    doc.LoadFile(_filePath.c_str());
    if (doc.ErrorID())
    {
        perror("Configuration::getConfigMap: LoadFile");
        exit(1);
    }
    _configMap.clear();
    tinyxml2::XMLElement *item = nullptr;

    string DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH; // Cppjieba五个文件路径
    string jieba_dict_path, hmm_model_path, idf_path, stop_words_path;    // simhash四个文件路径
    string WEB_PAGE_PATH, RI_PAGE_DAT, OFFSET_DAT;                        // 网页文件路径
    string NEW_RIPE_PAGE_DAT, NEW_OFFSET_DAT, INVERT_INDEX_DAT;           // 新增的配置项

    // JIEBA_DICT_UTF8

    // 获取根节点下的path节点
    tinyxml2::XMLElement *pathElement = doc.FirstChildElement("path");
    if (!pathElement)
    {
        perror("Cannot find path element");
        exit(1);
    }

    // 获取CPPJIEBA_PATH节点
    tinyxml2::XMLElement *cppjiebaElement = pathElement->FirstChildElement("CPPJIEBA_PATH");
    if (cppjiebaElement)
    {
        // JIEBA字典路径
        item = cppjiebaElement->FirstChildElement("DICT_PATH");
        if (item)
        {
            DICT_PATH = item->GetText();
        }

        // HMM模型路径
        item = cppjiebaElement->FirstChildElement("HMM_PATH");
        if (item)
        {
            HMM_PATH = item->GetText();
        }

        // 用户字典路径
        item = cppjiebaElement->FirstChildElement("USER_DICT_PATH");
        if (item)
        {
            USER_DICT_PATH = item->GetText();
        }

        // IDF路径
        item = cppjiebaElement->FirstChildElement("IDF_PATH");
        if (item)
        {
            IDF_PATH = item->GetText();
        }

        // 停用词路径
        item = cppjiebaElement->FirstChildElement("STOP_WORD_PATH");
        if (item)
        {
            STOP_WORD_PATH = item->GetText();
        }
    }

    // 获取simhash路径
    tinyxml2::XMLElement *simhashElement = pathElement->FirstChildElement("SIMHASH_PATH");
    if (simhashElement)
    {
        item = simhashElement->FirstChildElement("jieba_dict_path");
        if (item)
        {
            jieba_dict_path = item->GetText();
        }
        item = simhashElement->FirstChildElement("hmm_model_path");
        if (item)
        {
            hmm_model_path = item->GetText();
        }
        item = simhashElement->FirstChildElement("idf_path");
        if (item)
        {
            idf_path = item->GetText();
        }
        item = simhashElement->FirstChildElement("stop_words_path");
        if (item)
        {
            stop_words_path = item->GetText();
        }
    }

    // 网页库相关配置
    tinyxml2::XMLElement *webPage = pathElement->FirstChildElement("WEB_PAGE");
    if (webPage)
    {
        item = webPage->FirstChildElement("WEB_PAGE_PATH");
        if (item)
        {
            WEB_PAGE_PATH = item->GetText();
        }
        item = webPage->FirstChildElement("RI_PAGE_DAT");
        if (item)
        {
            RI_PAGE_DAT = item->GetText();
        }

        item = webPage->FirstChildElement("OFFSET_DAT");
        if (item)
        {
            OFFSET_DAT = item->GetText();
        }

        // 新增的配置项
        item = webPage->FirstChildElement("NEW_RIPE_PAGE_DAT");
        if (item)
        {
            NEW_RIPE_PAGE_DAT = item->GetText();
        }

        item = webPage->FirstChildElement("NEW_OFFSET_DAT");
        if (item)
        {
            NEW_OFFSET_DAT = item->GetText();
        }

        item = webPage->FirstChildElement("INVERT_INDEX_DAT");
        if (item)
        {
            INVERT_INDEX_DAT = item->GetText();
        }
    }

    _configMap = {
        {"DICT_PATH", DICT_PATH},
        {"HMM_PATH", HMM_PATH},
        {"USER_DICT_PATH", USER_DICT_PATH},
        {"IDF_PATH", IDF_PATH},
        {"STOP_WORD_PATH", STOP_WORD_PATH},

        {"jieba_dict_path", jieba_dict_path},
        {"hmm_model_path", hmm_model_path},
        {"idf_path", idf_path},
        {"stop_words_path", stop_words_path},

        {"WEB_PAGE_PATH", WEB_PAGE_PATH},
        {"RI_PAGE_DAT", RI_PAGE_DAT},
        {"OFFSET_DAT", OFFSET_DAT},
        {"NEW_RIPE_PAGE_DAT", NEW_RIPE_PAGE_DAT}, // 新增配置项
        {"NEW_OFFSET_DAT", NEW_OFFSET_DAT},       // 新增配置项
        {"INVERT_INDEX_DAT", INVERT_INDEX_DAT}    // 新增配置项
    };

    return _configMap;
}

// 获取停用词词集
set<string> Configuration::getStopWordList()
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(_filePath.c_str());
    if (doc.ErrorID())
    {
        perror("Configuration::getStopWordList: LoadFile");
        exit(1);
    }

    tinyxml2::XMLElement *item = nullptr;
    vector<string> paths;

    // 获取根节点下的path节点
    tinyxml2::XMLElement *pathElement = doc.FirstChildElement("path");
    if (!pathElement)
    {
        perror("Configuration::getStopWordList: Cannot find path element");
        exit(1);
    }

    tinyxml2::XMLElement *stopWordElement = pathElement->FirstChildElement("STOP_WORD_LIST");
    if (!stopWordElement)
    {
        perror("Configuration::getStopWordList: Cannot find STOP_WORD_LIST element");
        exit(1);
    }

    item = stopWordElement->FirstChildElement("STOP_WORDS_EN");
    if (item)
    {
        paths.push_back(item->GetText());
    }
    item = stopWordElement->FirstChildElement("STOP_WORDS_CN");
    if (item)
    {
        paths.push_back(item->GetText());
    }

    for (auto &path : paths)
    {
        ifstream ifs(path);
        if (!ifs)
        {
            perror("Configuration::getStopWordList: open file error");
        }

        string line;
        while (getline(ifs, line))
        {

            string stop;
            std::istringstream iss(line);
            while (iss >> stop)
            {
                for (char &c : stop)
                    if (c >= 'A' && c <= 'Z')
                        c |= 0x20;
                _stopWordList.insert(stop);
            }
        }

        ifs.close();
    }

    // 暂时返回空集合
    return _stopWordList;
}