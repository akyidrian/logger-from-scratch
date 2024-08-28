#pragma once
#include "log_stream.hpp"
#include <iostream>
#include <fstream>
#include <fmt/format.h>

class FileStream : public LogStream {
public:
    FileStream(LogLevel level, const std::string& filename) 
        : LogStream(level), m_file(filename, std::ios::app), fileOpened(m_file.is_open()) {
        if(!fileOpened)
        {
            std::cout << fmt::format("Failed to open log file: '{}'", filename) << '\n';
        }
    }

    void write(LogLevel level, const std::string& message) override {
        if (fileOpened && level >= getLogLevel()) {
            m_file << message << '\n';
        }
    }
private:
    std::ofstream m_file;
    bool fileOpened = false;
};
