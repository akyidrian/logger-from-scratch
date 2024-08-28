#pragma once
#include "log_stream.hpp"
#include <iostream>

class ConsoleStream : public LogStream {
public:
    ConsoleStream(LogLevel level = LogLevel::TRACE) : LogStream(level) {}
    void write(LogLevel level, const std::string& message) override;
};
