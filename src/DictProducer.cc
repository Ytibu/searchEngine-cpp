#include "DictProducer.h"

#include <dirent.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <string>

using std::cerr;
using std::cout;
using std::ifstream;
using std::ios;
using std::istringstream;
using std::ofstream;
using std::string;

/*英文词典建立过程：接收语料文件目录，目录转换为文件路径，将停用词接收*/
DictProducer::DictProducer(const string &dir, set<string> &stopWordList)
    : _stopWordList(stopWordList)
{
    getFiles(dir); // 将目录转变成文件列表
}

/*中文词典构造函数：接收语料文件目录，目录转换为文件路径，将停用词接收，接收分词工具*/
DictProducer::DictProducer(const string &dir, set<string> &stopWordList, SplitTool *splitTool)
    : _stopWordList(stopWordList), _splitTool(splitTool)
{
    getFiles(dir); // 将目录转变成文件列表
}

/*英文词典建立*/
void DictProducer::buildEnDict()
{
    for (auto &filePath : _files) // 遍历文件
    {
        cout << "buildEnDict  processing: " << filePath << "\n";

        ifstream ifs(filePath);
        if (!ifs)
        {
            perror("DictProducer::buildEnDict: open file error: ");
            continue;
        } // 文件打开检测

        string line;
        while (getline(ifs, line))
        {
            istringstream iss(line);
            string word;
            while (iss >> word) // 将每行的单词进行判断符合条件的最终插入容器
            {
                for (char &c : word)
                    if (c >= 'A' && c <= 'Z')
                        c |= 0x20;
                // 属于停用词，直接跳过此单词不处理
                if (_stopWordList.count(word))
                {
                    continue;
                }

                // 检测单词是否符合要求，标记
                bool isValidWord = true;
                for (size_t i = 0; i < word.size(); ++i)
                {
                    if (!isalpha(word[i]))
                    {
                        isValidWord = false;
                    }
                } // 判断是否是英文单词，做标记

                if (!isValidWord)
                {
                    continue;
                } // 删除非英文单词

                pushDict(word); // 过滤过的单词加入词典
            }
        }
        ifs.close();
    }

    // Sort only once after processing all files
    std::sort(_dict.begin(), _dict.end());
}

/*中文词典建立*/
void DictProducer::buildCnDict()
{
    for (auto &dir_file : _files)
    {
        cout << "buildCnDict  Processing: " << dir_file << "\n";

        ifstream ifs(dir_file, ios::ate);
        if (!ifs.good())
        {
            perror("DictProducer::buildCnDict: open file error");
            continue;
        }

        size_t length = ifs.tellg();
        ifs.seekg(std::ios_base::beg);
        char *buff = new char[length + 1];
        ifs.read(buff, length + 1);
        string txt(buff);

        vector<string> tmp = _splitTool->cut(txt);

        for (auto &i : tmp)
        {
            // 不是停用词且是utf8编码下的中文
            if (!_stopWordList.count(i) && getByteNum_Utf8(i[0]) == 3)
            {
                // 这是一个中文词，继续处理
                pushDict(i);
            }
        }
        ifs.close();
    }
    std::sort(_dict.begin(), _dict.end());
}

/*创建词典索引：中英文混合处理*/
void DictProducer::buildIndex()
{
    int i = 1;
    for (auto w : _dict)
    {
        string word = w.first;

        // 如果单词在停用词列表中，则跳过索引构建
        if (_stopWordList.count(word))
        {
            ++i;
            continue;
        }

        size_t charNums = word.size() / getByteNum_Utf8(word[0]);
        for (size_t idx = 0, n = 0; n != charNums; ++idx, ++n) // 得到字符数
        {
            // 按照字符个数切割
            size_t charLen = getByteNum_Utf8(word[idx]);
            string subWord = word.substr(idx, charLen); // 按照编码格式，进行拆分
            _index[subWord].insert(i);
            idx += (charLen - 1);
        }
        ++i;
    }
}

/*将词典添加到词典文件中*/
void DictProducer::storeDict(const char *filePath)
{
    ofstream ofs(filePath);
    if (!ofs)
    {
        perror("DictProducer::storeDict: open file error");
        return;
    }

    cout << "storeDict is loading: " << filePath << "\n";

    for (auto &word : _dict)
    {
        ofs << word.first << " " << word.second << "\n";
    }
    ofs.close();
}

/*将索引添加到索引文件中*/
void DictProducer::storeIndex(const char *filePath)
{
    ofstream ofs(filePath);
    if (!ofs)
    {
        perror("DictProducer::storeIndex: open file error");
        return;
    }

    cout << "storeIndex is loading: " << filePath << "\n";

    string record;
    for (auto &word : _index)
    {
        record = word.first;

        for (auto &index : word.second)
        {
            record += " " + std::to_string(index);
        }
        ofs << record << "\n";
    }
}

/*查看文件路径*/
void DictProducer::showFiles() const
{
    for (const auto &file : _files)
    {
        cout << file << "\n";
    }
}

/*显示词典内容*/
void DictProducer::showDict() const
{
    for (const auto &entry : _dict)
    {
        cout << entry.first << " " << entry.second << "\n";
    }
}

/*获取语料的绝对路径：根据构造函数传入的目录，获取目录下的所有文件存储在私有变量_files中*/
void DictProducer::getFiles(const string &dir)
{
    DIR *pDir = opendir(dir.c_str());
    if (!pDir)
    {
        perror("DictProducer::getFiles: opendir");
        return;
    }

    cout << dir << " is getFiles" << "\n";

    struct dirent *ptr;
    while ((ptr = readdir(pDir)) != 0)
    {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
        {
            _files.push_back(dir + "/" + ptr->d_name);
        }
    }

    closedir(pDir);
}

/*将单词添加到词典中*/
void DictProducer::pushDict(const std::string &word)
{
    /*确保过滤空字符串*/
    if (word.empty())
    {
        return;
    }

    /*直接遍历词典，如果找到频率加1*/
    size_t idx;
    for (idx = 0; idx < _dict.size(); ++idx)
    {
        if (word == _dict[idx].first)
        {
            ++_dict[idx].second;
            break; // 找到对应的词典对，直接终止循环
        }
    }

    /*根据遍历完后的索引可以判断出是否找到对应的词典对*/
    if (idx == _dict.size())
    {
        _dict.push_back(std::make_pair(word, 1));
    }
}

/* 获取utf8字符的字节 */
size_t DictProducer::getByteNum_Utf8(const char byte)
{
    int byteNum = 0;
    for (size_t i = 0; i < 6; ++i)
    {
        if (byte & (1 << (7 - i)))
            ++byteNum;
        else
            break;
    }
    return byteNum == 0 ? 1 : byteNum;
}
