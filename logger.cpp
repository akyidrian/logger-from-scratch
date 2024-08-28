#include "logger.hpp"
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>

Logger::Logger(LogLevel level, std::string format) 
    : m_level(level),
      m_format(std::move(format)),
      m_thread(&Logger::processQueue, this) {
}

Logger::~Logger() {
    m_exit = true;
    m_queueCv.notify_one();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Logger::addStream(std::unique_ptr<LogStream> stream) {
    std::unique_lock<std::mutex> _(m_writeMutex);
    m_streams.push_back(std::move(stream));
}

void Logger::setFormat(const std::string& format) {
    std::unique_lock<std::mutex> _(m_writeMutex);
    m_format = format;
}

void Logger::setLogLevel(LogLevel level) {
    m_level = level;
}

bool Logger::shouldLog(LogLevel level) const {
    return level >= m_level;
}

void Logger::processQueue() {
    while (!m_exit) {
        std::unique_lock<std::mutex> queueLock(m_queueMutex);
        m_queueCv.wait(queueLock, [this] { return !m_queue.empty() || m_exit; });

        while (!m_queue.empty()) {
            auto [level, message] = m_queue.front();
            m_queue.pop();
            queueLock.unlock();
            {
                std::unique_lock<std::mutex> _(m_writeMutex);
                std::string formatted_message = formatMessage(level, message);
                for (const auto& stream : m_streams) {
                    // We assume stream->write() is lightweight and doesn't
                    // incur significant wait...
                    stream->write(level, formatted_message);
                }
            }
            queueLock.lock();
        }
    }
}

const char* Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::formatMessage(LogLevel level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    return fmt::format(fmt::runtime(m_format),
        fmt::arg("timestamp", now),
        fmt::arg("level", levelToString(level)),
        fmt::arg("message", message)
    );
}
