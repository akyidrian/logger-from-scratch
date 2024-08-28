#include "Logger.h"
#include "FileStream.h"
#include "ConsoleStream.h"

int main() {
    Logger logger("{timestamp:%Y-%m-%d %H:%M:%S} <{level}> {message}", LogLevel::TRACE);
    logger.addStream(std::make_unique<FileStream>("app.log", LogLevel::DEBUG));
    logger.addStream(std::make_unique<ConsoleStream>(LogLevel::INFO));

    logger.trace("This will only be logged to the file");
    logger.debug("This will be logged to the file but not to the console");
    logger.info("This will be logged to both file and console");
    logger.warning("Warning message");
    logger.error("Error message");
    logger.critical("Critical error message");

    // Change log levels
    logger.setLogLevel(LogLevel::WARNING);
    if (auto stream = logger.getStream(0)) {
        stream->setLogLevel(LogLevel::ERROR);
    }
    if (auto stream = logger.getStream(1)) {
        stream->setLogLevel(LogLevel::CRITICAL);
    }

    logger.info("This won't be logged anywhere");
    logger.warning("This will only be logged to the console");
    logger.error("This will be logged to both file and console");
    logger.critical("This will be logged to both file and console");

    return 0;
}