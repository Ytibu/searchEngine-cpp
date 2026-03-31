#include "PageLibPreprocessor.h"
#include "DirScanner.h"
#include "WebPage.h"

#include "../include/tinyxml/tinyxml2.h"
#include "../include/simhash/Simhasher.hpp"

#include <iostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <unordered_set>

#include <sys/param.h>

using std::cerr;
using std::cout;
using std::ifstream;
using std::make_pair;
using std::ofstream;

PageLibPreprocessor::PageLibPreprocessor(Configuration &config, SplitToolCppJieba &jieba)
    : _conf(config), _jieba(jieba)
{
    /* 读取网页库和位置偏移库的内容  */
    readInfoFromFile();
}

// 预处理
void PageLibPreprocessor::doProcess()
{
    cutRedundantPages();
    buildInvertIndexTable();
    storeOnDisk();
}

// 根据配置信息读取网页库和位置偏移库的内容
void PageLibPreprocessor::readInfoFromFile()
{
    string element;
    map<std::string, std::string> confFile = _conf.getConfigMap();

    // 检查必要的配置键是否存在
    if (confFile.count("OFFSET_DAT") == 0)
    {
        perror("PageLibPreprocessor::readInfoFromFile: Missing OFFSET_DAT configuration");
        return;
    }
    if (confFile.count("RI_PAGE_DAT") == 0)
    {
        perror("PageLibPreprocessor::readInfoFromFile: Missing RI_PAGE_DAT configuration");
        return;
    }

    ifstream ifsOFF(confFile.at("OFFSET_DAT"));
    if (!ifsOFF)
    {
        perror("PageLibPreprocessor::readInfoFromFile: open offset file error");
        return;
    }

    // 以二进制模式打开网页库文件
    ifstream ifsWeb(confFile.at("RI_PAGE_DAT"), std::ios::binary);
    if (!ifsWeb)
    {
        perror("PageLibPreprocessor::readInfoFromFile: open ripage file error");
        return;
    }

    string line;
    while (getline(ifsOFF, line))
    {
        int id, start, length; // 使用streamsize存储长度

        std::stringstream ss(line);
        if (!(ss >> id >> start >> length))
        {
            perror("PageLibPreprocessor::readInfoFromFile: parse offset file error");
            continue; // 跳过错误行
        }

        // 定位到指定位置
        ifsWeb.seekg(start, std::ios::beg);
        if (!ifsWeb)
        {
            perror("PageLibPreprocessor::readInfoFromFile: seek to position error");
            continue;
        }

        // 分配缓冲区并读取指定长度的数据
        vector<char> buffer(length + 1); // +1 for null terminator
        ifsWeb.read(buffer.data(), length);

        // 检查实际读取的字节数
        size_t bytesRead = ifsWeb.gcount();
        if (bytesRead != length)
        {
            perror("Error: Failed to read from file");
        }
        // 确保字符串以null结尾
        buffer[MIN(bytesRead, length)] = '\0';
        // 转换为string
        string info(buffer.data(), bytesRead);

        // cout << "Reading file: " << info << "\n";

        WebPage webPage(info, _conf, _jieba);
        _pageLib.push_back(webPage);
    }
}

// 对冗余的网页进行去重
void PageLibPreprocessor::cutRedundantPages()
{
    std::sort(_pageLib.begin(), _pageLib.end());
    // 排序

    auto last = std::unique(_pageLib.begin(), _pageLib.end());
    // 重复元素排序到容器后端，并返回重复元素在后端的起点迭代器

    _pageLib.erase(last, _pageLib.end());
    // 删除重复元素：将起点迭代器到末尾删除

    map<std::string, std::string> confFile = _conf.getConfigMap();

    // 检查必要的配置键是否存在
    if (confFile.count("OFFSET_DAT") == 0)
    {
        perror("PageLibPreprocessor::cutRedundantPages: Missing OFFSET_DAT configuration");
        return;
    }

    ifstream ifs(confFile.at("OFFSET_DAT"));
    if (!ifs)
    {
        perror("PageLibPreprocessor::cutRedundantPages: open offset file error");
        return;
    }

    // 优化：一次性将偏移信息加载到内存中，避免重复遍历文件
    map<int, pair<int, int>> offsetMap;
    string line;
    while (getline(ifs, line))
    {
        std::stringstream ss(line);
        int docid, offset, length;
        if (ss >> docid >> offset >> length)
        {
            offsetMap[docid] = make_pair(offset, length);
        }
    }

    for (auto &webPage : _pageLib)
    {
        int oldDocId = webPage.getDocId();
        auto it = offsetMap.find(oldDocId);
        if (it != offsetMap.end())
        {
            _offsetLib[oldDocId] = it->second;
        }
    }
}

// 创建倒排索引表
void PageLibPreprocessor::buildInvertIndexTable()
{
    // Step 1: 统计每个词的文档频率 DF
    unordered_map<string, int> df; // Document Frequency
    for (auto &webPage : _pageLib)
    {
        map<string, int> &wordsMap = webPage.getWordsMap();
        for (auto &wordPair : wordsMap)
        {
            df[wordPair.first]++;
        }
    }

    // Step 2: 计算 IDF 和 TF-IDF 权重
    int N = _pageLib.size(); // 文档总数

    for (auto &webPage : _pageLib)
    {
        map<string, int> &wordsMap = webPage.getWordsMap();
        int docId = webPage.getDocId();

        // 计算该文档中所有词的权重平方和，用于归一化
        unordered_map<string, double> tfIdfWeights;
        double sumSquaredWeights = 0.0;

        // 先计算每个词的TF-IDF权重
        for (auto &wordPair : wordsMap)
        {
            string word = wordPair.first;
            int tf = wordPair.second; // Term Frequency

            // 计算 IDF = log2(N/(DF+1))
            double idf = log2((double)N / (df[word] + 1));

            // 计算 TF-IDF 权重
            double weight = tf * idf;
            tfIdfWeights[word] = weight;

            // 累加平方用于归一化
            sumSquaredWeights += weight * weight;
        }

        // 归一化处理
        double normFactor = sqrt(sumSquaredWeights);
        for (auto &weightPair : tfIdfWeights)
        {
            string word = weightPair.first;
            double normalizedWeight = weightPair.second;
            if (normFactor > 0)
            {
                normalizedWeight /= normFactor;
            }

            // 将该词对应的文档ID和归一化权重加入倒排索引
            _invertIndexTable[word].push_back(std::make_pair(docId, normalizedWeight));
        }
    }

    // Step 3: 对每个词的文档列表按文档ID排序
    for (auto &entry : _invertIndexTable)
    {
        std::sort(entry.second.begin(), entry.second.end(),
                  [](const pair<int, double> &a, const pair<int, double> &b)
                  {
                      return a.first < b.first;
                  });
    }
}

// 将经过预处理之后的网页库、位置偏移库和倒排索引表写回到磁盘上
void PageLibPreprocessor::storeOnDisk()
{
    map<std::string, std::string> confFile = _conf.getConfigMap();

    // 检查必要的配置键是否存在
    if (confFile.count("NEW_RIPE_PAGE_DAT") == 0)
    {
        perror("PageLibPreprocessor::storeOnDisk: Missing NEW_RIPE_PAGE_DAT configuration");
        return;
    }
    if (confFile.count("NEW_OFFSET_DAT") == 0)
    {
        perror("PageLibPreprocessor::storeOnDisk: Missing NEW_OFFSET_DAT configuration");
        return;
    }
    if (confFile.count("INVERT_INDEX_DAT") == 0)
    {
        perror("PageLibPreprocessor::storeOnDisk: Missing INVERT_INDEX_DAT configuration");
        return;
    }

    // 写入去重后的网页库
    ofstream ofsWeb(confFile.at("NEW_RIPE_PAGE_DAT"), std::ios::binary);
    if (!ofsWeb)
    {
        perror("PageLibPreprocessor::storeOnDisk: open webpage file error");
        return;
    }

    // 写入去重后的偏移库
    ofstream ofsOffset(confFile.at("NEW_OFFSET_DAT"));
    if (!ofsOffset)
    {
        perror("PageLibPreprocessor::storeOnDisk: open offset file error");
        return;
    }

    // 写入倒排索引表
    ofstream ofsInvertIndex(confFile.at("INVERT_INDEX_DAT"));
    if (!ofsInvertIndex)
    {
        perror("PageLibPreprocessor::storeOnDisk: open invert index file error");
        return;
    }

    int offset = 0;
    int docId = 0;
    for (auto &webPage : _pageLib)
    {
        std::string doc = webPage.getDoc();
        ofsWeb.write(doc.data(), doc.size());

        // 写入偏移信息 (docid, offset, length)
        ++docId;
        int length = doc.size();
        ofsOffset << docId << " " << offset << " " << length << "\n";
        offset += length;
    }

    // 写入倒排索引表到文件
    // 格式：词项 文档频率 docId,权重 docId,权重 ...
    for (auto &entry : _invertIndexTable)
    {
        string word = entry.first;
        vector<pair<int, double>> &docList = entry.second;

        // 写入词项
        ofsInvertIndex << word << " ";
        // 写入文档频率（包含该词的文档数量）
        ofsInvertIndex << docList.size();
        // 写入文档ID和权重
        for (auto &docPair : docList)
        {
            ofsInvertIndex << " " << docPair.first << "," << docPair.second;
        }
        // 每个词项的信息写入一行
        ofsInvertIndex << "\n";
    }

    ofsWeb.close();
    ofsOffset.close();
    ofsInvertIndex.close();
}

// SplitToolCppJieba _jieba;    // 分词对象
// vector <WebPage> _pageLib;  // 网页库容器的对象
// unordered_map<int, pair<int, int> > _offsetLib; //网页偏移库对象
// unordered_map<string, vector<pair<int, double>>> _invertIndexTable; //倒排索引表对象