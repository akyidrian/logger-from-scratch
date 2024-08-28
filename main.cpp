#include <console_stream.hpp>
#include <logger.hpp>
#include <file_stream.hpp>

int main() {
    Logger logger;
    logger.addStream(std::make_unique<ConsoleStream>(LogLevel::DEBUG));
    logger.addStream(std::make_unique<FileStream>(LogLevel::DEBUG, "./my-log-file.txt"));

    logger.trace("This will NOT be logged");
    logger.debug("This will be logged to BOTH");
    logger.info("This will be logged to BOTH");
    logger.warning("This will be logged to BOTH");
    logger.error("This will be logged to BOTH");
    logger.critical("This will be logged to BOTH");

    logger.setFormat("<{level}> {timestamp:%H:%M:%S} {message}");
    logger.setLogLevel(LogLevel::WARNING);
    if (auto consoleStream = logger.getStream(0)) {
        consoleStream->setLogLevel(LogLevel::ERROR);
    }
    if (auto fileStream = logger.getStream(1)) {
        fileStream->setLogLevel(LogLevel::CRITICAL);
    }

    logger.trace("This will NOT be logged");
    logger.debug("This will NOT be logged");
    logger.debug("This will NOT be logged");
    logger.info("This will NOT be logged");
    logger.warning("This will NOT be logged");
    logger.error("This will be logged to CONSOLE");
    logger.critical("This will be logged to BOTH");

    logger.setFormat("{message}");
    logger.removeStream(0);
    logger.critical("This will be logged to FILE");

    return 0;
}
