# Search Project

## 项目开发环境

- Linux: Ubuntu 24.04
- g++/gcc: Version 13.3.0

## 系统目录结构

```
search-engine/
├── bin/                  # 可执行程序
├── conf/
│   └── myconf.conf       # 配置信息
├── data/
│   ├── dict.dat          # 词典
│   ├── dictIndex.dat     # 单词索引库
│   ├── newripepage.dat   # 网页库
│   ├── newoffset.dat     # 网页偏移库
│   └── invertIndex.dat   # 倒排索引库
├── include/              # 头文件（*.hpp）
├── log/                  # 日志文件
├── src/                  # 源文件（*.cpp/*.cc）
├── source/               # 资源文件（中文、英文语料、网页、停用词等）  
│   ├── art/
│   ├── eng/
│   ├── web/
│   └── websource/
└── README.md             # 项目说明
```

---
