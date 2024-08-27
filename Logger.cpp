#include "Logger.h"
#include <chrono>
#include <fmt/format.h>
#include <fmt/chrono.h>

Logger::Logger(std::string format, LogLevel minLevel) 
    : m_format(std::move(format)), m_minLevel(minLevel) {
    m_thread = std::thread(&Logger::processEntries, this);
}

Logger::~Logger() {
    m_exit = true;
    m_cv.notify_one();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Logger::addStream(std::unique_ptr<LogStream> stream) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_streams.push_back(std::move(stream));
}

void Logger::setFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_format = format;
}

void Logger::setLogLevel(LogLevel level) {  // Renamed from setMinLogLevel
    m_minLevel = level;
}

bool Logger::shouldLog(LogLevel level) const {
    return level >= m_minLevel;
}

void Logger::processEntries() {
    while (!m_exit) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_queue.empty() || m_exit; });

        while (!m_queue.empty()) {
            auto [level, message] = m_queue.front();
            m_queue.pop();

            std::string formatted_message = formatMessage(level, message);

            for (const auto& stream : m_streams) {
                if (level >= stream->getLogLevel()) {
                    stream->write(formatted_message);
                }
            }
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
    return fmt::format(m_format,
        fmt::arg("timestamp", now),
        fmt::arg("level", levelToString(level)),
        fmt::arg("message", message)
    );
}