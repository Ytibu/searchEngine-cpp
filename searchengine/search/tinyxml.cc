#include "../include/tinyxml/tinyxml2.h"
#include <iostream>
#include <string>
using std::cout;
using namespace tinyxml2;
using std::cerr;
using std::endl;
using std::string;

void test()
{

    XMLDocument doc;
    doc.LoadFile("../conf/myconf.conf");
    if (doc.ErrorID())
    {
        cerr << "LoadFile Error" << endl;
        return;
    }

    XMLElement *itemNode = doc.FirstChildElement("path")->FirstChildElement("SIMHASH_PATH")->FirstChildElement("JIEBA_DICT_UTF8");
    if (itemNode)
    {
        // string title = itemNode->FirstChildElement("title")->GetText();
        string title = itemNode->GetText();
        cout << title << endl;
    }
}

int main()
{
    test();
}