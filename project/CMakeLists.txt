# CMake settings
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Project settings
project(project)

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
find_package(OpenAL REQUIRED)
find_package(FFTW REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${IL_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${OPENAL_INCLUDE_DIRS})
include_directories(${FFTW_INCLUDE_DIRS})

include_directories("${CMAKE_SOURCE_DIR}")
link_directories("${CMAKE_BINARY_DIR}/util")
link_directories("${CMAKE_BINARY_DIR}/r2tk")

# Compile
#set(LIBRARIES ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${IL_LIBRARIES} r2tk util)
set(LIBRARIES ${OPENAL_LIBRARIES} ${FFTW_LIBRARIES} r2tk util)
set(HEADERS sound.hpp entity.hpp filter.hpp)
set(SOURCES main.cpp sound.cpp entity.cpp filter.cpp)
add_executable(project ${HEADERS} ${SOURCES})

# Link
target_link_libraries(project ${LIBRARIES})

# Copy resources on build
add_custom_target(project_resources ALL
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Copy external binaries on build
add_custom_target(project_externals ALL
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/external/bin ${project_BINARY_DIR})