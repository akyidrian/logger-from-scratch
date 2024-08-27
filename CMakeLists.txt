cmake_minimum_required(VERSION 3.10)

project(LoggerProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add fmt library
include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)
FetchContent_MakeAvailable(fmt)

# Add the logger library
add_library(Logger STATIC
    Logger.cpp
    FileStream.cpp
    ConsoleStream.cpp
)

# Include directories for the library
target_include_directories(Logger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link fmt library
target_link_libraries(Logger PUBLIC fmt::fmt)

# Add the executable
add_executable(LoggerApp
    main.cpp
)

# Link the executable with the logger library
target_link_libraries(LoggerApp PRIVATE Logger)