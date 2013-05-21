#include "shader.hpp"
#include <r2tk/r2-exception.hpp>
#include <memory>
#include <fstream>
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

		throw r2ExceptionIOM("Failed to compile shader: " + log);
    }
}

Shader::~Shader() {
    GLCheck(glDeleteShader(m_id));
}

std::shared_ptr<Shader> Shader::loadShader(const std::string& filename, GLenum shaderType) {
	std::ifstream file(filename.c_str());

	if (!file.is_open()) {
        throw r2ExceptionIOM("Failed to open shader: " + filename);
    }

	std::string source;
	while (!file.eof()) {
		std::string line;
		std::getline(file, line);

		source += line + "\n";
	}

	return std::shared_ptr<Shader>(new Shader(source.c_str(), shaderType));
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

