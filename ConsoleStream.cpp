#include "ConsoleStream.h"
#include <iostream>

void ConsoleStream::write(LogLevel level, const std::string& message) {
    if (level >= getLogLevel()) {
        std::cout << message << std::endl;
    }
}