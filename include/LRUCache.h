#ifndef __LRUCACHE_H__
#define __LRUCACHE_H__ 

#include <list>
#include <unordered_map>
#include <string>
#include <map>
using std::string;
using std::unordered_map;
using std::list;
using std::pair;

/**
 * 缓存类
 */
class LRUCache {
public:
    LRUCache(int num = 1000);
    LRUCache(const LRUCache & cache);
    void addElement(const string &key, const string &value);  //向缓存中添加数据
    void readFromFile(const string & fileName);  //从文件中读取缓存信息
    void writeToFile(const string &filename);   //将缓存信息写入文件
    void update(const LRUCache &rhs);  //更新缓存信息
    list<string,string> &getPendingUpdateList();    //获取待更新的节点List

private:
    list<string, string> _resultsList;  //保存键值对
    //unordered_map<string,iterator> _hashMap;  //采用hashTable进行查找
    unordered_map<string,list<pair<string, string>>> _hashMap;
    list<string, string> _pengdingUpdateList;   //等待更新的节点信息
    int _capacity;  //缓存节点容量
};

#endif