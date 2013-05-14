# Find the GLM library
#
# Defines:
# GLM_INCLUDE_DIR: The path to the include files.
# GLM_FOUND: True if the include files and the library has been found.

find_path(OPENAL_INCLUDE_DIR 
          AL/al.h
          HINTS "${CMAKE_SOURCE_DIR}/external/include")
find_library(OPENAL_LIBRARY 
		  NAMES OpenAL32 
		  HINTS "${CMAKE_SOURCE_DIR}/external/lib")

set(OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
set(OPENAL_LIBRARIES ${OPENAL_LIBRARY})

mark_as_advanced(OPENAL_INCLUDE_DIR OPENAL_LIBRARY)