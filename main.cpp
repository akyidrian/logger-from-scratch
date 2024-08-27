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

    return 0;
}