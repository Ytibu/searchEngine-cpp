#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__ 

#include <vector>
#include <string>
#include <map>
#include <set>
using std::vector;
using std::string;
using std::map;
using std::set;


/**
 * 词典类
 */

class Dictionary{
public:
    static Dictionary *createInstance();   //静态函数
    void init(const string &dictpath);    //通过词典文件路径初始化词典
    vector<std::pair<string, int>> &getDict(); //获取词典
    map<string, set<int>> &getIndexTable(); //获取索引表
    string doQuery(string word);    //执行查询

private:
    vector<std::pair<string,int>> _dict;    //词典
    map<string,set<int>> _indexTable;   //索引表
};

#endif
