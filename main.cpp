#include <console_stream.hpp>
#include <logger.hpp>
#include <log_stream.hpp>
#include <file_stream.hpp>
#include <memory>

void defaultDebuggerLogEachLevel()
{
    logger::trace("trace");
    logger::debug("debug");
    logger::info("info");
    logger::warning("warning");
    logger::error("error");
    logger::critical("critical");
}

void debuggerLogEachLevel(logger::Logger& logger)
{
    logger.trace("trace");
    logger.debug("debug");
    logger.info("info");
    logger.warning("warning");
    logger.error("error");
    logger.critical("critical");
}

int main() {
    logger::critical("-- Printing trace->critical with default logger...");
    defaultDebuggerLogEachLevel();

    logger::log(LogLevel::CRITICAL, "====================");

    // FIXME: Add api to make constructing default logger with specified streams cleaner
    logger::critical("-- Making new default (file) logger)...");
    auto myDefaultLogger = std::make_shared<logger::Logger>(LogLevel::INFO, "{message}");
    myDefaultLogger->addStream(std::make_unique<FileStream>(LogLevel::DEBUG, "./default-log.txt"));
    myDefaultLogger->removeStream(0);  // Removing defaultly constructed console stream
    logger::setDefaultLogger(myDefaultLogger);
    logger::critical("-- Making new default (file) logger)...");
    defaultDebuggerLogEachLevel();


    logger::log(LogLevel::CRITICAL, "--==================");
    logger::log(LogLevel::CRITICAL, "--==================");

    logger::Logger myLogger;
    myLogger.log(LogLevel::CRITICAL, "**==================");
    myLogger.log(LogLevel::CRITICAL, "**==================");
    myLogger.critical("-- Printing trance->critical with my (console) logger...");
    debuggerLogEachLevel(myLogger);

    myLogger.log(LogLevel::CRITICAL, "====================");

    myLogger.addStream(std::make_unique<FileStream>(LogLevel::INFO, "./my-log.txt"));
    myLogger.critical("-- Adding file stream to my logger...");
    debuggerLogEachLevel(myLogger);

    myLogger.log(LogLevel::CRITICAL, "====================");

    myLogger.critical("-- Changing format on my logger...");
    myLogger.setFormat("<{level}> {timestamp:%H:%M:%S} {message}");

    myLogger.log(LogLevel::CRITICAL, "====================");

    myLogger.critical("-- Printing error->critical with my (console) logger...");
    myLogger.critical("-- Printing critical with my (file) logger...");
    myLogger.setLogLevel(LogLevel::WARNING);
    if (auto consoleStream = myLogger.getStream(0)) {
        consoleStream->setLogLevel(LogLevel::ERROR);
    }
    if (auto fileStream = myLogger.getStream(1)) {
        fileStream->setLogLevel(LogLevel::CRITICAL);
    }
    debuggerLogEachLevel(myLogger);

    myLogger.log(LogLevel::CRITICAL, "====================");

    myLogger.critical("-- Removing console stream...");
    myLogger.removeStream(0);
    myLogger.critical("-- Printing critical with my (file) logger...");
    myLogger.setFormat("{message}");
    debuggerLogEachLevel(myLogger);

    return 0;
}
