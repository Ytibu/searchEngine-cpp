#include "Configuration.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <string>

#include "yaml-cpp/yaml.h"

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
    // 如果已经加载过配置，清空内容，重新加载配置
    if (!_configMap.empty())
    {
        _configMap.clear();
    }

    try
    {
        YAML::Node config = YAML::LoadFile(_filePath);
        std::map<std::string, std::string> _configMap;

        const YAML::Node cppjieba = config["CPPJIEBA_PATH"];
        const YAML::Node simhash = config["SIMHASH_PATH"];
        const YAML::Node stopword = config["STOP_WORD_LIST"];
        const YAML::Node webpage = config["WEB_PAGE"];

        if (!cppjieba || !simhash || !stopword || !webpage)
        {
            std::cerr << "configuration section is missing in " << _filePath << '\n';
            return;
        }

        _configMap["DICT_PATH"] = cppjieba["DICT_PATH"] ? cppjieba["DICT_PATH"].as<std::string>() : throw std::runtime_error("missing required key: DICT_PATH");
        _configMap["HMM_PATH"] = cppjieba["HMM_PATH"] ? cppjieba["HMM_PATH"].as<std::string>() : throw std::runtime_error("missing required key: HMM_PATH");
        _configMap["USER_DICT_PATH"] = cppjieba["USER_DICT_PATH"] ? cppjieba["USER_DICT_PATH"].as<std::string>() : throw std::runtime_error("missing required key: USER_DICT_PATH");
        _configMap["IDF_PATH"] = cppjieba["IDF_PATH"] ? cppjieba["IDF_PATH"].as<std::string>() : throw std::runtime_error("missing required key: IDF_PATH");
        _configMap["STOP_WORD_PATH"] = cppjieba["STOP_WORD_PATH"] ? cppjieba["STOP_WORD_PATH"].as<std::string>() : throw std::runtime_error("missing required key: STOP_WORD_PATH");

        _configMap["jieba_dict_path"] = simhash["jieba_dict_path"] ? simhash["jieba_dict_path"].as<std::string>() : throw std::runtime_error("missing required key: jieba_dict_path");
        _configMap["hmm_model_path"] = simhash["hmm_model_path"] ? simhash["hmm_model_path"].as<std::string>() : throw std::runtime_error("missing required key: hmm_model_path");
        _configMap["idf_path"] = simhash["idf_path"] ? simhash["idf_path"].as<std::string>() : throw std::runtime_error("missing required key: idf_path");

        _configMap["STOP_WORDS_EN"] = stopword["STOP_WORDS_EN"] ? stopword["STOP_WORDS_EN"].as<std::string>() : throw std::runtime_error("missing required key: STOP_WORDS_EN");
        _configMap["STOP_WORDS_CN"] = stopword["STOP_WORDS_CN"] ? stopword["STOP_WORDS_CN"].as<std::string>() : throw std::runtime_error("missing required key: STOP_WORDS_CN");

        _configMap["WEB_PAGE_PATH"] = webpage["WEB_PAGE_PATH"] ? webpage["WEB_PAGE_PATH"].as<std::string>() : throw std::runtime_error("missing required key: WEB_PAGE_PATH");
        _configMap["RI_PAGE_DAT"] = webpage["RI_PAGE_DAT"] ? webpage["RI_PAGE_DAT"].as<std::string>() : throw std::runtime_error("missing required key: RI_PAGE_DAT");
        _configMap["OFFSET_DAT"] = webpage["OFFSET_DAT"] ? webpage["OFFSET_DAT"].as<std::string>() : throw std::runtime_error("missing required key: OFFSET_DAT");
        _configMap["NEW_RIPE_PAGE_DAT"] = webpage["NEW_RIPE_PAGE_DAT"] ? webpage["NEW_RIPE_PAGE_DAT"].as<std::string>() : throw std::runtime_error("missing required key: NEW_RIPE_PAGE_DAT");
        _configMap["NEW_OFFSET_DAT"] = webpage["NEW_OFFSET_DAT"] ? webpage["NEW_OFFSET_DAT"].as<std::string>() : throw std::runtime_error("missing required key: NEW_OFFSET_DAT");
        _configMap["NEW_WEB_PAGE_PATH"] = webpage["NEW_WEB_PAGE_PATH"] ? webpage["NEW_WEB_PAGE_PATH"].as<std::string>() : throw std::runtime_error("missing required key: NEW_WEB_PAGE_PATH");
    }
    catch (const YAML::BadFile &e)
    {
        std::cerr << "failed to open " << _filePath << ": " << e.what() << '\n';
        return;
    }
    catch (const YAML::Exception &e)
    {
        std::cerr << "yaml parse error: " << e.what() << '\n';
        return;
    }
    catch (const std::exception &e)
    {
        std::cerr << "config error: " << e.what() << '\n';
        return;
    }

    return _configMap;
}

// 获取停用词词集
set<string> Configuration::getStopWordList()
{

    vector<string> paths;
    try
    {
        YAML::Node config = YAML::LoadFile(_filePath);
        std::map<std::string, std::string> _configMap;

        const YAML::Node stopword = config["STOP_WORD_LIST"];

        if (!stopword)
        {
            std::cerr << "configuration section is missing in " << _filePath << '\n';
            return;
        }

        if (stopword["STOP_WORDS_EN"])
        {
            paths.push_back(stopword["STOP_WORDS_EN"].as<std::string>());
        }
        else
        {
            throw std::runtime_error("missing required key: STOP_WORDS_EN");
        }

        if (stopword["STOP_WORDS_CN"])
        {
            paths.push_back(stopword["STOP_WORDS_CN"].as<std::string>());
        }
        else
        {
            throw std::runtime_error("missing required key: STOP_WORDS_CN");
        }
    }
    catch (const YAML::BadFile &e)
    {
        std::cerr << "failed to open " << _filePath << ": " << e.what() << '\n';
        return;
    }
    catch (const YAML::Exception &e)
    {
        std::cerr << "yaml parse error: " << e.what() << '\n';
        return;
    }
    catch (const std::exception &e)
    {
        std::cerr << "config error: " << e.what() << '\n';
        return;
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