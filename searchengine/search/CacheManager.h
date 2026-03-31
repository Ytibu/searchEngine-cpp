#ifndef __CACHEMANAGER_H__
#define __CACHEMANAGER_H__ 

#include <vector>
//#include <unixHeader.h>
#include "LRUCache.h"

class CacheManager {
public:
    void initCache(size_t, const string &filename); //从磁盘文件中读取缓存信息
    LRUCache &getCache(size_t idx); //获取某个缓存
    void periodicUpdateCaches();    //定时更新所有缓存
    
private:
    std::vector<LRUCache> _cacheList;    //缓存的数量与线程个数一致
};

#endif