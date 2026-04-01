#include "Logger.hpp"
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>

Logger::Logger()
    : _root(log4cpp::Category::getRoot())
{
    auto ptn1 = new log4cpp::PatternLayout();
    ptn1->setConversionPattern("%d %c [%p] - %m%n");
    auto ptn2 = new log4cpp::PatternLayout();
    ptn2->setConversionPattern("%d %c [%p] - %m%n");

    /*创建两个appender，一个为通用输出流，一个为回滚文件输出，各自绑定一个布局样式*/
    auto appenderOut = new log4cpp::OstreamAppender("console", &std::cout);
    auto appenderRollingFile = new log4cpp::RollingFileAppender("../log/rollingfile", "../log/rollingfile.log", 5 * 1024, 9);
    appenderOut->setLayout(ptn1);
    appenderRollingFile->setLayout(ptn2);

    /*设置日志级别，一个记录器绑定两个输出器，减少多余创建的开销*/
    _root.setPriority(log4cpp::Priority::DEBUG);
    _root.addAppender(appenderOut);
    _root.addAppender(appenderRollingFile);
}

Logger::~Logger()
{
    log4cpp::Category::shutdown();
}

// 使用C++11局部静态变量方式实现线程安全的单例
/*开辟一个局部静态变量，返回引用，二次初始化只会返回引用，不会实际初始化*/
Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}