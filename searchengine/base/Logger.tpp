#include "Logger.hpp"

// 模板函数定义
template <typename... T>
void Logger::warn(const char *msg, const T &...args)
{
    _root.warn(msg, args...);
}

template <typename... T>
void Logger::debug(const char *msg, const T &...args)
{
    _root.debug(msg, args...);
}

template <typename... T>
void Logger::info(const char *msg, const T &...args)
{
    _root.info(msg, args...);
}

template <typename... T>
void Logger::error(const char *msg, const T &...args)
{
    _root.error(msg, args...);
}