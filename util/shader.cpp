#include "shader.hpp"
#include <memory>
#include <cstdio>

Shader::Shader(const char* source, GLenum shaderType) {
    m_id = GLCheck(glCreateShader(shaderType));
    GLCheck(glShaderSource(m_id, 1, &source, NULL));
    GLCheck(glCompileShader(m_id));

    GLint compiled;
    GLCheck(glGetShaderiv(m_id, GL_COMPILE_STATUS, &compiled));
    if (compiled == GL_FALSE) {
        GLint logSize;
        GLCheck(glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &logSize));

        std::string log;
        if (logSize > 0) {
            int charsWritten;
            std::unique_ptr<char> logBuffer(new char[logSize]);
            GLCheck(glGetShaderInfoLog(m_id, logSize, &charsWritten, logBuffer.get()));
            
            log = logBuffer.get();
        }

        std::cerr << "ERROR: Failed to compile shader: " << log << std::endl;
    }
}

Shader::~Shader() {
    GLCheck(glDeleteShader(m_id));
}

std::shared_ptr<Shader> Shader::loadShader(const std::string& filename, GLenum shaderType) {
    FILE* f = fopen(filename.c_str(), "r");
    if (f == NULL) {
        std::cerr << "ERROR: Failed to open shader: " << filename << std::endl;
        return std::shared_ptr<Shader>();
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    rewind(f);

    std::unique_ptr<char> source(new char[length + 1]);
    size_t result = fread(source.get(), 1, length, f);
    if (result != length) {
        std::cerr << "ERROR: Failed to read shader: " << filename << std::endl;
        return std::shared_ptr<Shader>();
    }

    source.get()[length] = '\0';

    fclose(f);

    return std::shared_ptr<Shader>(new Shader(source.get(), shaderType));
}


Program::Program(const std::vector<std::shared_ptr<Shader> >& shaders) {
    m_id = GLCheck(glCreateProgram());

    for (int i = 0; i < shaders.size(); ++i) {
		m_shaders.push_back(shaders[i]);
        GLCheck(glAttachShader(m_id, shaders[i]->getId()));
	}

    GLCheck(glLinkProgram(m_id));
}

Program::~Program() {
    for (int i = 0; i < m_shaders.size(); ++i) {
        GLCheck(glDetachShader(m_id, m_shaders[i]->getId()));
	}

    GLCheck(glDeleteProgram(m_id));
}

