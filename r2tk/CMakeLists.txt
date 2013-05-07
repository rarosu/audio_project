# CMake settings
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Project settings
project(r2tk)

# Compiler settings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Using Clang
    message("Setting Clang flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Using GCC
    message("Setting GCC flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Using MSVC
else()
    message("WARNING: Using unrecognized compiler, not setting flags")
endif()

# Compile
set(HEADERS r2-global.hpp r2-exception.hpp r2-assert.hpp r2-singleton.hpp r2-observer.hpp)
set(SOURCES r2-exception.cpp r2-assert.cpp)
add_library(r2tk STATIC ${HEADERS} ${SOURCES})
