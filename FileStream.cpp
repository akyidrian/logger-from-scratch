#include "FileStream.h"

FileStream::FileStream(const std::string& filename, LogLevel level) 
    : LogStream(level), m_file(filename, std::ios::app) {}

void FileStream::write(const std::string& message) {
    if (m_file.is_open()) {
        m_file << message << std::endl;
    }
}