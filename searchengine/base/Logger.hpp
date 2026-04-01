#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <log4cpp/Category.hh>
#include <string>
#include <sstream>
#include <cstdarg>
using std::string;

/**
 * 日志类
 * 单例模式：确保只有一个实例，并且提供全局访问点，使用静态方法获取实例
 * 宏定义：日志打印宏定义，方便调用
 */

#define LOG_WARN(format, ...) Logger::getInstance().warn(format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Logger::getInstance().error(format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Logger::getInstance().info(format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) Logger::getInstance().debug(format, ##__VA_ARGS__)

class Logger
{
public:
    // 添加接受std::string参数的重载版本，以兼容现有调用方式
    void warn(const std::string& msg) { _root.warn(msg); }
    void debug(const std::string& msg) { _root.debug(msg); }
    void info(const std::string& msg) { _root.info(msg); }
    void error(const std::string& msg) { _root.error(msg); }

    // 模板函数声明
    template <typename... T>
    void warn(const char *msg, const T &...args);
    
    template <typename... T>
    void debug(const char *msg, const T &...args);
    
    template <typename... T>
    void info(const char *msg, const T &...args);
    
    template <typename... T>
    void error(const char *msg, const T &...args);


    static Logger &getInstance();

private:
    Logger(); 
    ~Logger();
    Logger(const Logger &) = delete;    
    Logger &operator=(const Logger&)=delete;    

private:
    log4cpp::Category &_root;
};

#include "Logger.tpp"
#endif