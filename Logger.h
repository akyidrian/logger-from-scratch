#pragma once
#include "LogStream.h"
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <fmt/format.h>
#include <fmt/chrono.h>

class Logger {
public:
    Logger(std::string format = "{timestamp} [{level}] {message}", LogLevel minLevel = LogLevel::TRACE);
    ~Logger();

    void addStream(std::unique_ptr<LogStream> stream);
    
    template<typename... Args>
    void log(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args);
    
    // Convenience methods for each log level
    template<typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::TRACE, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warning(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::WARNING, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::CRITICAL, fmt, std::forward<Args>(args)...);
    }
    
    void setFormat(const std::string& format);
    void setLogLevel(LogLevel level);  // Renamed from setMinLogLevel

private:
    std::vector<std::unique_ptr<LogStream>> m_streams;
    std::queue<std::pair<LogLevel, std::string>> m_queue;  // Changed to store LogLevel and message
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_thread;
    std::atomic<bool> m_exit{false};
    std::string m_format;
    std::atomic<LogLevel> m_minLevel;

    static const char* levelToString(LogLevel level);
    void processEntries();
    std::string formatMessage(LogLevel level, const std::string& message);
    bool shouldLog(LogLevel level) const;
};

template<typename... Args>
void Logger::log(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args) {
    if (shouldLog(level)) {
        std::string message = fmt::format(fmt, std::forward<Args>(args)...);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.emplace(level, std::move(message));
        }
        m_cv.notify_one();
    }
}