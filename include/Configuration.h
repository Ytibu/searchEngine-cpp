#ifndef __Configuration_H__
#define __Configuration_H__

#include <map>
#include <string>
#include <set>
using std::map;
using std::string;
using std::set;


class Configuration
{
public:
    static Configuration &getInstance(const string& filePath);
    map<string, string> &getConfigMap();   //获取存放配置文件内容的map
    set<string> getStopWordList(); //获取英文停用词词集

    // 禁止拷贝构造和赋值
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    
private:
    Configuration(const string& filePath);  //导入文件路径，构造
    ~Configuration();


private:
    string _filePath;   //配置文件路径
    map<string, string> _configMap; //配置文件内容
    set<string> _stopWordList;//停用词词集
};

#endif