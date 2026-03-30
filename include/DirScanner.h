#ifndef __DIRSCANNER_H__
#define __DIRSCANNER_H__ 

#include <vector>
#include <string>
using std::string;
using std::vector;

/**
 * 目录扫描类--递归扫描
 */

class DirScanner{
public:
    DirScanner() = default;
    void operator()(const string &dirname);  //重载函数调用运算符，调用 traverse函数
    vector<string> &getFiles(); //返回_vecFilesfiles 的引用
    void traverse(const string &dirname);   //获取某一目录下的所有文件

private:
    vector<string> _files;  //存放每个语料文件的绝对路径

};

#endif