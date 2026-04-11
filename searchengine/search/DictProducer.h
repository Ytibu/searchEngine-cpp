#include "SplitTool.h"

#include <vector>
#include <string>
#include <map>
#include <vector>
using std::pair;
using std::string;
using std::map;
using std::set;
using std::vector;


/**
 * 词典创建类
 */



class DictProducer {
public:
    DictProducer(const string &dir, const set<string> &stopWordList);
    DictProducer(const string &dir, const set<string> &stopWordList, SplitTool *splitTool);  //中文词典构造函数
    
    void buildEnDict(); //创建英文词典
    void buildCnDict(); //创建中文词典
    void buildIndex();  //创建索引
    void storeDict(const char *filePath);   //词典写入文件
    void storeIndex(const char *filePath);  //索引写入文件
    void showFiles()const;  //查看文件路径（测试）
    void showDict()const;   //查看词典（测试）

private:
    void getFiles(const string &dir);    //获取文件的绝对路径
    void pushDict(const string &word); //添加词典(存储某个单词)
    size_t getByteNum_Utf8(const unsigned char byte);//获取utf8字符字节数


private:
    vector<pair<string,int>> _dict;  //词典(词语,频数)
    map<string, set<int>> _index;  //词典索引（单词,词典索引）
    SplitTool *_splitTool = nullptr;  //分词工具
    vector<string> _files;  //语料库文件的路径（根据构造函数传入的目录，获取目录下的文件路径）
    set<string> _stopWordList;

};