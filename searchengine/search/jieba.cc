#include <iostream>
#include "../include/cppjieba/Jieba.hpp"
#include <vector>
#include <string>
using std::cout;
using std::string;
using std::vector;

const char *const DICT_PATH = "../include/dict/jieba.dict.utf8";
const char *const HMM_PATH = "../include/dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "../include/dict/user.dict.utf8";
const char *const IDF_PATH = "../include/dict/idf.utf8";
const char *const STOP_WORD_PATH = "../include/dict/stop_words.utf8";

int main(int argc, char *argv[])
{
    cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    // cppjieba::Jieba jieba;
    // vector<string> words;
    // vector<cppjieba::Word> jiebawords;
    string s = "Wecom to Beijing北京欢迎你";
    vector<string> out;
    jieba.Cut(s, out);

    for (const auto &word : out)
    {
        cout << word << "\n";
    }
    string result;
}