# Find the GLM library
#
# Defines:
# GLM_INCLUDE_DIR: The path to the include files.
# GLM_FOUND: True if the include files and the library has been found.

find_path(GLM_INCLUDE_DIR 
          glm/glm.hpp
          HINTS "${CMAKE_SOURCE_DIR}/external/include")

set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})

mark_as_advanced(GLM_INCLUDE_DIR)