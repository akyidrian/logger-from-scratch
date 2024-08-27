#include "ConsoleStream.h"
#include <iostream>

void ConsoleStream::write(const std::string& message) {
    std::cout << message << std::endl;
}