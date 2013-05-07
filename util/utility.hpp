#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>

// Log an OpenGL error
#define logError(code, line) \
    if (code != GL_NO_ERROR) \
        std::cerr << "ERROR (" << __FILE__ << ":" << line << "): " << gluErrorString(code) << std::endl; \

// Check for errors in debug mode
#define GLCheck(F) F; { int line = __LINE__; \
        int error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            logError(error, line); \
        } \
    }


#endif
