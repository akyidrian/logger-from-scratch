cmake_minimum_required(VERSION 3.22)

project(logger VERSION 1.0)

if(NOT UNIX)
  message(FATAL_ERROR "Linux is the only supported platform")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 11.0.2)
FetchContent_MakeAvailable(fmt)

add_library(logger STATIC logger.cpp file_stream.cpp console_stream.cpp)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(logger PUBLIC fmt::fmt)

add_executable(logger-app main.cpp)
target_link_libraries(logger-app PRIVATE logger)
