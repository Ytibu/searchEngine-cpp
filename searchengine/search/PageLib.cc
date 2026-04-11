#include "PageLib.h"

#include "../base/Configuration.h"
#include "DirScanner.h"

#include "../../include/tinyxml/tinyxml2.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>

using std::cerr;
using std::cout;
using std::hash;
using std::ifstream;
using std::map;
using std::ofstream;
using std::regex;
using std::string;
using std::to_string;

PageLib::PageLib(Configuration &configuration, DirScanner &dirScanner)
    : _dirScanner(dirScanner)
{
    // 获取配置，获取网页存放位置
    map<string, string> config = configuration.getConfigMap();
    _WebPagePath["RI_PAGE_DAT"] = config["RI_PAGE_DAT"];     // 网页库索引文件路径
    _WebPagePath["OFFSET_DAT"] = config["OFFSET_DAT"];       // 网页库偏移文件路径
    _WebPagePath["WEB_PAGE_PATH"] = config["WEB_PAGE_PATH"]; // 网页文件目录路径
}

void PageLib::create() // 创建网页库
{
    _pages.clear(); // 为网页库容器添加内容前确保容器为空
    size_t i = 0;   // 网页库的索引docid

    _dirScanner(_WebPagePath["WEB_PAGE_PATH"]);
    vector<string> &fileNameList = _dirScanner.getFiles();

    size_t start = 0;
    for (auto &fileName : fileNameList)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(fileName.c_str());
        if (doc.Error())
        {
            perror("PageLib::create: Error parsing file ");
            continue;
        }

        tinyxml2::XMLElement *item = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("item");
        if (!item)
        {
            perror("PageLib::create: item is null");
        }

        // 若有item元素，读取内部元素内容，拼接成字符串，加入格式化容器中
        while (item)
        {
            std::regex reg1(R"(<!\[CDATA\[(.*?)\]\]>)"); // 正则
            std::regex reg2("<[^>]*>");

            tinyxml2::XMLElement *titleElement = item->FirstChildElement("title");
            tinyxml2::XMLElement *linkElement = item->FirstChildElement("link");
            tinyxml2::XMLElement *descElement = item->FirstChildElement("description");

            string title = regex_replace(titleElement->GetText() ? titleElement->GetText() : "", reg1, "");
            string url = linkElement->GetText() ? linkElement->GetText() : "";
            string content = descElement->GetText() ? descElement->GetText() : "";
            content = regex_replace(content, reg2, "");
            string docid = to_string(++i);

            string fmtTXT = string("<doc>\n") +
                            "<docid>" + docid + "</docid>\n" +
                            "<url>" + url + "</url>\n" +
                            "<title>" + title + "</title>\n" +
                            "<content>" + content + "</content>\n</doc>\n";
            size_t length = fmtTXT.length();

            _offsetLib.insert({i, std::pair<size_t, size_t>(start, length)});
            start += length;

            _pages.push_back(fmtTXT); // 将格式化后的网页加入容器

            // 移动到下一个item元素，避免无限循环
            item = item->NextSiblingElement("item");
        }
    }
}

// 存储网页库和网页偏移库
void PageLib::store() const
{
    // 检查必要的配置键是否存在
    if (_WebPagePath.count("RI_PAGE_DAT") == 0)
    {
        perror("PageLib::store: Missing RI_PAGE_DAT configuration");
        return;
    }
    if (_WebPagePath.count("OFFSET_DAT") == 0)
    {
        perror("PageLib::store: Missing OFFSET_DAT configuration");
        return;
    }

    string _ripageDat = _WebPagePath.at("RI_PAGE_DAT");
    std::ofstream ofs(_ripageDat);
    if (!ofs)
    {
        perror("PageLib::store: opendir error");
        return;
    }
    for (const auto &page : _pages)
    {
        ofs << page;
    }

    string offsetPath = _WebPagePath.at("OFFSET_DAT");
    std::ofstream ofs2(offsetPath);
    if (!ofs2)
    {
        perror("PageLib::store: opendir error");
        return;
    }
    for (const auto &offset : _offsetLib)
    {
        ofs2 << offset.first << " " << offset.second.first << " " << offset.second.second << "\n";
    }

    ofs.close();
    ofs2.close();
}