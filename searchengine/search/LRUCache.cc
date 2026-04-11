#include "LRUCache.h"

LRUCache::LRUCache(int num = 1000)
    :_capacity(num)
{
}
LRUCache::LRUCache(const LRUCache &cache)
    :_capacity(cache._capacity)
{
}

// 向缓存中添加数据
void LRUCache::addElement(const string &key, const string &value)
{

}

// 从文件中读取缓存信息
void LRUCache::readFromFile(const string &fileName)
{

}

// 将缓存信息写入文件
void LRUCache::writeToFile(const string &filename)
{

}

// 更新缓存信息
void LRUCache::update(const LRUCache &rhs)
{

}                        

// 获取待更新的节点List
list<string, string> &LRUCache::getPendingUpdateList()
{

}