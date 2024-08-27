#pragma once
#include <string>

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class LogStream {
public:
    LogStream(LogLevel level = LogLevel::TRACE) : m_level(level) {}
    virtual ~LogStream() = default;
    virtual void write(const std::string& message) = 0;
    void setLogLevel(LogLevel level) { m_level = level; }
    LogLevel getLogLevel() const { return m_level; }

protected:
    LogLevel m_level;
};