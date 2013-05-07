# Find the GLEW library
#
# Defines:
# GLEW_LIBRARY: The name of the library.
# GLEW_INCLUDE_DIR: The path to the include files.
# GLEW_FOUND: True if the include files and the library has been found.

find_path(GLEW_INCLUDE_DIR 
          GL/glew.h
          HINTS "${CMAKE_SOURCE_DIR}/external/include")
find_library(GLEW_LIBRARY 
             NAMES GLEW glew glew32 
             PATH_SUFFIXES lib64
             HINTS "${CMAKE_SOURCE_DIR}/external/lib")

set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
set(GLEW_LIBRARIES ${GLEW_LIBRARY})

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)