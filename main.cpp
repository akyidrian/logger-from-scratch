#include <console_stream.hpp>
#include <logger.hpp>
#include <file_stream.hpp>

int main() {
    Logger logger(LogLevel::TRACE, "{timestamp:%Y-%m-%d %H:%M:%S} <{level}> {message}");
    logger.addStream(std::make_unique<ConsoleStream>(LogLevel::DEBUG));
    logger.addStream(std::make_unique<FileStream>(LogLevel::DEBUG, "log.txt"));

    logger.trace("This will NOT be logged");
    logger.debug("This will be logged to BOTH");
    logger.info("This will be logged to BOTH");
    logger.warning("This will be logged to BOTH");
    logger.error("This will be logged to BOTH");
    logger.critical("This will be logged to BOTH");

    logger.setFormat("{timestamp:%Y-%m-%d %H:%M:%S} ({level}) {message}");
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

    return 0;
}
