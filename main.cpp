#include <console_stream.hpp>
#include <logger.hpp>
#include <log_stream.hpp>
#include <file_stream.hpp>
#include <memory>

// FIXME: Allow default stream to be replaced?
int main() {
    logger::trace("trace");
    logger::debug("debug");
    logger::info("info");
    logger::warning("warning");
    logger::error("error");
    logger::critical("critical");
    logger::log(LogLevel::INFO, "====================");
    logger::setDefaultLogger(std::make_shared<logger::Logger>(LogLevel::INFO, "{message}"));
    logger::trace("trace");
    logger::debug("debug");
    logger::info("info");
    logger::warning("warning");
    logger::error("error");
    logger::critical("critical");
    logger::log(LogLevel::INFO, "====================");

    logger::Logger myLogger;
    myLogger.addStream(std::make_unique<FileStream>(LogLevel::DEBUG, "./my-log-file.txt"));

    myLogger.trace("This will be logged to CONSOLE");
    myLogger.debug("This will be logged to BOTH");
    myLogger.info("This will be logged to BOTH");
    myLogger.warning("This will be logged to BOTH");
    myLogger.error("This will be logged to BOTH");
    myLogger.critical("This will be logged to BOTH");
    myLogger.log(LogLevel::INFO, "====================");

    myLogger.setFormat("<{level}> {timestamp:%H:%M:%S} {message}");
    myLogger.setLogLevel(LogLevel::WARNING);
    if (auto consoleStream = myLogger.getStream(0)) {
        consoleStream->setLogLevel(LogLevel::ERROR);
    }
    if (auto fileStream = myLogger.getStream(1)) {
        fileStream->setLogLevel(LogLevel::CRITICAL);
    }

    myLogger.trace("This will NOT be logged");
    myLogger.debug("This will NOT be logged");
    myLogger.debug("This will NOT be logged");
    myLogger.info("This will NOT be logged");
    myLogger.warning("This will NOT be logged");
    myLogger.error("This will be logged to CONSOLE");
    myLogger.critical("This will be logged to BOTH");
    myLogger.log(LogLevel::INFO, "====================");

    myLogger.setFormat("{message}");
    myLogger.removeStream(0);
    myLogger.critical("This will be logged to FILE");

    return 0;
}
