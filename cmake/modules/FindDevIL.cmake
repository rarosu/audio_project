# Find the DevIL library
#
# Defines:
# IL_LIBRARY: The name of the library.
# IL_INCLUDE_DIR: The path to the include files.
# IL_FOUND: True if the include files and the library has been found.

find_path(IL_INCLUDE_DIR 
          IL/il.h
          HINTS "${CMAKE_SOURCE_DIR}/external/include")
find_library(IL_LIBRARY 
             NAMES DevIL IL
             PATH_SUFFIXES lib64
             HINTS "${CMAKE_SOURCE_DIR}/external/lib")

set(IL_INCLUDE_DIRS ${IL_INCLUDE_DIR})
set(IL_LIBRARIES ${IL_LIBRARY})

mark_as_advanced(IL_INCLUDE_DIR IL_LIBRARY)
