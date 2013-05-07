# This package creates a static library, compiled using C++11. As such, it requires a C++11 aware compiler.

# CMake settings
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Project settings
project(util)

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

# Find required libraries
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(DevIL REQUIRED)
find_package(GLM REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${IL_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

include_directories("${CMAKE_SOURCE_DIR}")
link_directories("${CMAKE_BINARY_DIR}/r2tk")

# Compile
set(LIBRARIES ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${IL_LIBRARIES} r2tk)
set(HEADERS util.hpp utility.hpp shader.hpp template.hpp buffer.hpp camera.hpp texture.hpp mesh.hpp material.hpp)
set(SOURCES shader.cpp template.cpp buffer.cpp camera.cpp texture.cpp mesh.cpp material.cpp)
add_library(util STATIC ${HEADERS} ${SOURCES})

# Link
target_link_libraries(util ${LIBRARIES})
