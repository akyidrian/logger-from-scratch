#include <console_stream.hpp>
#include <logger.hpp>
#include <file_stream.hpp>

int main() {
    Logger logger(LogLevel::TRACE, "{timestamp:%Y-%m-%d %H:%M:%S} <{level}> {message}");
    logger.addStream(std::make_unique<FileStream>(LogLevel::DEBUG, "log.txt"));
    logger.addStream(std::make_unique<ConsoleStream>(LogLevel::DEBUG));

    logger.trace("This will not be logged");
    logger.debug("This will be logged to the file and console");
    logger.info("This will be logged to both file and console");
    logger.warning("This will be logged to both file and console");
    logger.error("This will be logged to both file and console");
    logger.critical("This will be logged to both file and console");

    // FIXME: Setting the following doesn't guarentee it won't affect
    //        earlier log messages. We'd have to force a flush in the
    //        logger. Arguably, any setLogLevel on the streams should
    //        take immediate effect.
    logger.setFormat("{timestamp:%Y-%m-%d %H:%M:%S} [{level}] {message}");
    logger.setLogLevel(LogLevel::WARNING);
    if (auto stream = logger.getStream(0)) {
        stream->setLogLevel(LogLevel::ERROR);
    }
    if (auto stream = logger.getStream(1)) {
        stream->setLogLevel(LogLevel::CRITICAL);
    }

    logger.trace("This will not be logged");
    logger.debug("This will not be logged");
    logger.debug("This will not be logged");
    logger.info("This will not be logged");
    logger.warning("This will not be logged");
    logger.error("This will be logged to file");
    logger.critical("This will be logged to both file and console");

    return 0;
}
