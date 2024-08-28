#pragma once
#include "log_stream.hpp"
#include <atomic>
#include <condition_variable>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <queue>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Logger {
public:
    Logger(LogLevel level = LogLevel::TRACE, std::string format = "{timestamp} [{level}] {message}");
    ~Logger();

    void setFormat(const std::string& format);
    void setLogLevel(LogLevel level);

    void addStream(std::unique_ptr<LogStream> stream);
    LogStream* getStream(size_t index) {
        if (index < m_streams.size()) {
            return m_streams[index].get();
        }
        return nullptr;
    }

    template<typename... Args>
    void log(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args);

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

private:
    bool shouldLog(LogLevel level) const;

    std::atomic<bool> m_exit{false};
    std::atomic<LogLevel> m_level;

    std::mutex m_writeMutex;
    std::string m_format;
    std::vector<std::unique_ptr<LogStream>> m_streams;

    std::mutex m_queueMutex;
    std::condition_variable m_queueCv;
    std::queue<std::pair<LogLevel, std::string>> m_queue;

    std::thread m_thread;

    static const char* levelToString(LogLevel level);
    void processQueue();
    std::string formatMessage(LogLevel level, const std::string& message);
};

template<typename... Args>
void Logger::log(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args) {
    if (shouldLog(level)) {
        std::string message = fmt::format(fmt, std::forward<Args>(args)...);
        {
            std::lock_guard<std::mutex> _(m_queueMutex);
            m_queue.emplace(level, std::move(message));
        }
        m_queueCv.notify_one();
    }
}
