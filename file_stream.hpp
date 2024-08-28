#pragma once
#include "log_stream.hpp"
#include <fstream>

class FileStream : public LogStream {
public:
    FileStream(const std::string& filename, LogLevel level = LogLevel::TRACE);
    void write(LogLevel level, const std::string& message) override;

private:
    std::ofstream m_file;
};
