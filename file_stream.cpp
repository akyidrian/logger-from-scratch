#include "file_stream.hpp"

FileStream::FileStream(const std::string& filename, LogLevel level) 
    : LogStream(level), m_file(filename, std::ios::app) {}

void FileStream::write(LogLevel level, const std::string& message) {
    if (level >= getLogLevel() && m_file.is_open()) {
        m_file << message << std::endl;
    }
}
