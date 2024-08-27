#pragma once
#include "LogStream.h"
#include <fstream>

class FileStream : public LogStream {
public:
    FileStream(const std::string& filename, LogLevel level = LogLevel::TRACE);
    void write(const std::string& message) override;

private:
    std::ofstream m_file;
};