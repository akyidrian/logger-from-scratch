#include "logger.hpp"
#include "console_stream.hpp"
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>

using namespace logger;

namespace {
    static std::shared_ptr<Logger> defaultLogger = std::make_unique<Logger>();
}; // namespace

std::shared_ptr<Logger> logger::getDefaultLogger() {
    return defaultLogger;
}

void logger::setDefaultLogger(std::shared_ptr<Logger> logger)
{
    defaultLogger = logger;
}

Logger::Logger(LogLevel level, std::string format) 
    : m_filterLevel(level),
      m_format(std::move(format)),
      m_thread(&Logger::processQueue, this) {
    m_streams.emplace_back(std::make_unique<ConsoleStream>());
}

Logger::~Logger() {
    m_exit = true;
    m_queueCv.notify_one();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Logger::addStream(std::unique_ptr<LogStream> stream) {
    std::unique_lock<std::mutex> _(m_streamsMutex);
    m_streams.push_back(std::move(stream));
}

void Logger::removeStream(size_t index) {
    std::lock_guard<std::mutex> _(m_streamsMutex);
    if (index < m_streams.size()) {
        auto stream = m_streams.begin() + index;
        flushToStream(*stream);
        m_streams.erase(stream);
    }
}

LogStream* Logger::getStream(size_t index) {
    std::lock_guard<std::mutex> _(m_streamsMutex);
    if (index < m_streams.size()) {
        return m_streams[index].get();
    }
    return nullptr;
}

void Logger::setLogLevel(LogLevel level) {
    std::unique_lock<std::mutex> _(m_settingsMutex);
    flushQueue();
    m_filterLevel = level;
}

void Logger::setFormat(const std::string& format) {
    std::unique_lock<std::mutex> _(m_settingsMutex);
    flushQueue();
    m_format = format;
}

// This function is called by removeStream. It assumes the caller
// has acquired the m_streamMutex.
void Logger::flushToStream(std::unique_ptr<LogStream>& stream) {
    // We lock until the queue is completely flushed
    // to block any potential new logs being added
    std::unique_lock<std::mutex> queueLock(m_queueMutex);
    for(auto [level, message] : m_queue) {
        stream->write(level, message);
    }
}

void Logger::flushQueue() {
    // We lock until the queue is completely flushed
    // to block any potential new logs being added
    std::unique_lock<std::mutex> queueLock(m_queueMutex);
    while (!m_queue.empty()) {
        auto [level, message] = m_queue.front();
        m_queue.pop_front();
        {
            std::unique_lock<std::mutex> _(m_streamsMutex);
            for (const auto& stream : m_streams) {
                // We assume stream->write() is lightweight...
                stream->write(level, message);
            }
        }
    }
}

void Logger::processQueue() {
    while (!m_exit) {
        std::unique_lock<std::mutex> queueLock(m_queueMutex);
        m_queueCv.wait(queueLock, [this] { return !m_queue.empty() || m_exit; });

        while (!m_queue.empty()) {
            auto [level, message] = m_queue.front();
            m_queue.pop_front();
            queueLock.unlock();
            {
                std::unique_lock<std::mutex> _(m_streamsMutex);
                for (const auto& stream : m_streams) {
                    // We assume stream->write() is lightweight...
                    stream->write(level, message);
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

std::string Logger::formatMessage(LogLevel level, const std::string format, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    return fmt::format(fmt::runtime(format),
        fmt::arg("timestamp", now),
        fmt::arg("level", levelToString(level)),
        fmt::arg("message", message)
    );
}
