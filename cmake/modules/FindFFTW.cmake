# Find the FFTW library
#
# Defines:
# FFTW_INCLUDE_DIR: The path to the include files.
# FFTW_LIBRARY: The library file
# FFTW_FOUND: True if the include files and the library has been found.

find_path(FFTW_INCLUDE_DIR 
          fftw3.h
          HINTS "${CMAKE_SOURCE_DIR}/external/include")
find_library(FFTW_LIBRARY 
		  NAMES libfftw3-3
		  HINTS "${CMAKE_SOURCE_DIR}/external/lib")

set(FFTW_INCLUDE_DIRS ${FFTW_INCLUDE_DIR})
set(FFTW_LIBRARIES ${FFTW_LIBRARY})

mark_as_advanced(FFTW_INCLUDE_DIR FFTW_LIBRARY)