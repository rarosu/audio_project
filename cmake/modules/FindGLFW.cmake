# Find the GLFW library
#
# Defines:
# GLFW_LIBRARY: The name of the library.
# GLFW_INCLUDE_DIR: The path to the include files.
# GLFW_FOUND: True if the include files and the library has been found.

find_path(GLFW_INCLUDE_DIR 
          GL/glfw.h
          HINTS "${CMAKE_SOURCE_DIR}/external/include")
find_library(GLFW_LIBRARY 
             NAMES GLFW glfw 
             PATH_SUFFIXES lib64
             HINTS "${CMAKE_SOURCE_DIR}/external/lib")

set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
set(GLFW_LIBRARIES ${GLFW_LIBRARY})

mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)