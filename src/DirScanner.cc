#include "DirScanner.h"

#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include <string>
#include <iostream>

using std::cerr;
using std::cout;

// 重载函数调用运算符，调用 traverse函数
void DirScanner::operator()(const string &dirname)
{
    traverse(dirname);
}

// 返回_vecFilesfiles 的引用
vector<string> &DirScanner::getFiles()
{
    return _files;
}

// 获取某一目录下的所有文件
void DirScanner::traverse(const string &dirname)
{
    DIR *pDir = opendir(dirname.c_str());
    if (!pDir)
    {
        perror("DirScanner::traverse: opendir");
        return;
    }

    cout << dirname << " is scanned" << "\n";

    struct dirent *ptr;
    while ((ptr = readdir(pDir)) != 0)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }

        // 构造绝对路径
        std::string fullPath = dirname + "/" + ptr->d_name;

        struct stat statBuf;
        if (stat(fullPath.c_str(), &statBuf) == -1)
        {
            perror("DirScanner::traverse: stat error");
            continue;
        }
        if (S_ISDIR(statBuf.st_mode))
        {
            traverse(fullPath);
        }
        else
        {
            _files.push_back(fullPath);
        }
    }

    closedir(pDir);
}
