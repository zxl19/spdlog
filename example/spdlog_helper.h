/**
 * @file spdlog_helper.h
 * @author zxl19
 * @brief Helper classes and functions for spdlog, which enable calling spdlog in a glog way.
 * @version 0.1
 * @date 2025-08-14
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#ifndef SPDLOG_HELPER_H
#define SPDLOG_HELPER_H

#include <sstream>
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

// 初始化spdlog，替代glog的初始化
inline void init_spdlog_async(const std::string& log_file = "logs/app.log",
                              size_t queue_size = 8192,
                              size_t thread_count = 1,
                              bool console_output = true) {
    // 初始化异步日志
    spdlog::init_thread_pool(queue_size, thread_count);

    std::vector<spdlog::sink_ptr> sinks;

    // 根据参数决定是否添加控制台输出
    if (console_output) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        sinks.emplace_back(console_sink);
    }

    // 添加文件输出
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
    file_sink->set_level(spdlog::level::trace);
    sinks.emplace_back(file_sink);

    // 创建多目标日志器
    auto logger = std::make_shared<spdlog::async_logger>("multi_logger", sinks.begin(), sinks.end(),
                                                         spdlog::thread_pool(),
                                                         spdlog::async_overflow_policy::block);

    // 设置日志级别和格式
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    // log line format: [IWEF]mmdd hh:mm:ss.uuuuuu threadid file:line] msg
    // logger->set_pattern("%^[%L%m%d %T.%f %t %s:%#] %v%$");

    // 注册为默认日志器
    spdlog::set_default_logger(logger);

    // 每隔3秒刷新一次日志
    spdlog::flush_every(std::chrono::seconds(3));
}

// 定义日志级别枚举
enum LogSeverity {
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    FATAL = 3,
};

namespace google {

// 模拟glog的初始化函数
inline void InitGoogleLogging(const char* argv0) {
    // 默认初始化，只输出到文件
    init_spdlog_async(argv0);
}

// 重载初始化函数，允许控制是否输出到控制台
inline void InitGoogleLogging(const char* argv0, bool console_output) {
    init_spdlog_async(argv0, 8192, 1, console_output);
}

// 模拟glog的关闭函数
inline void ShutdownGoogleLogging() { spdlog::shutdown(); }

}  // namespace google

// 定义LOG宏，重定向到spdlog
#define LOG(LEVEL) \
    if (LEVEL >= INFO && LEVEL <= FATAL) LogWrapper<LEVEL>().stream()

// 日志包装器模板
template <int Level>
class LogWrapper {
public:
    // 默认构造函数
    LogWrapper() = default;

    // 析构函数
    ~LogWrapper() {
        switch (Level) {
            case INFO:
                SPDLOG_INFO(stream_.str());
                break;
            case WARNING:
                SPDLOG_WARN(stream_.str());
                break;
            case ERROR:
                SPDLOG_ERROR(stream_.str());
                break;
            case FATAL:
                SPDLOG_CRITICAL(stream_.str());
                std::abort();  // glog的FATAL会终止程序
                break;
        }
    }

    // 拷贝构造函数
    LogWrapper(const LogWrapper& other) = delete;

    // 拷贝赋值运算符
    LogWrapper& operator=(const LogWrapper& other) = delete;

    // 移动构造函数
    LogWrapper(LogWrapper&& other) noexcept = delete;

    // 移动赋值运算符
    LogWrapper& operator=(LogWrapper&& other) noexcept = delete;

    std::ostream& stream() { return stream_; }

private:
    std::ostringstream stream_;
};

#endif
