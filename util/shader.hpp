#ifndef SHADER_HPP
#define SHADER_HPP

#include <memory>
#include <vector>
#include <string>
#include <GL/glew.h>
#include "utility.hpp"

class Shader {
public:
    /** Expects the source as a NULL-terminated c-string */
    Shader(const char* source, GLenum shaderType);
    ~Shader();

    /** Load a shader from a file */
    static std::shared_ptr<Shader> loadShader(const std::string& filename, GLenum shaderType);

    GLuint getId() const { return m_id; }
private:
    GLuint m_id;

    Shader(const Shader&);
    Shader& operator=(const Shader&);
};

class Program {
public:
    Program(const std::vector<std::shared_ptr<Shader> >& shaders);
    ~Program();

    GLuint getId() const { return m_id; }
private:
    GLuint m_id;
    std::vector<std::shared_ptr<Shader> > m_shaders;

    Program(const Program&);
    Program& operator=(const Program&);
};

class glUseProgramState {
public:
    glUseProgramState(GLuint id) { GLCheck(glUseProgram(id)); }
    ~glUseProgramState() { GLCheck(glUseProgram(0)); }
};

#endif
