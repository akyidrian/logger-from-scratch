#pragma once
#include "LogStream.h"

class ConsoleStream : public LogStream {
public:
    ConsoleStream(LogLevel level = LogLevel::TRACE) : LogStream(level) {}
    void write(const std::string& message) override;
};