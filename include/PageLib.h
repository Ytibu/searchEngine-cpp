#ifndef __PAGELIB_H__
#define __PAGELIB_H__ 

#include "Configuration.h"
#include "DirScanner.h"

#include <vector>
#include <string>
#include <map>

using std::map;
using std::string;
using std::vector;
using std::map;

/**
 * 网页库类
 */

class PageLib {
public:
    PageLib(Configuration& conf, DirScanner& dirScanner);
    void store() const;   // 存储网页库和网页偏移库
    void create();  // 创建网页库


private:
    map<std::string,std::string> _WebPagePath;   // 配置文件内容
    DirScanner & _dirScanner;   // 扫描目录对象的引用
    vector<string> _pages;  //存放格式化网页的容器
    map<int, std::pair<int, int> > _offsetLib;   //存放网页的偏移量（每篇文档在网页库的位置）
};

#endif