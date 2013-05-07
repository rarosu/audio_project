# CMake settings
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Project settings
project(audio_project)

# Add custom module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Compile the subprojects
add_subdirectory(r2tk)
add_subdirectory(util)
add_subdirectory(project)
