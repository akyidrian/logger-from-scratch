#pragma once
#include "log_stream.hpp"
#include <iostream>
#include <fstream>
#include <fmt/format.h>

class FileStream : public LogStream {
public:
    FileStream(LogLevel level, const std::string& filename) 
        : LogStream(level), m_file(filename, std::ios::app) {
        if(!m_file.is_open())
        {
            std::cout << fmt::format("Failed to open log file: '{}'", filename) << '\n';
        }
    }

    void write(LogLevel level, const std::string& message) override {
        // FIXME: Must ensure m_file.is_open() in case error occurred during construction
        if (m_file.is_open() && level >= getLogLevel()) {
            m_file << message << '\n';
        }
    }
private:
    std::ofstream m_file;
};
