#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <memory>
#include <GL/glew.h>
#include "utility.hpp"

/** Defines a vertex buffer object (VBO) */
class VBO {
public:
    VBO();
    ~VBO();

    GLuint getId() const { return m_id; }
private:
    GLuint m_id;

    VBO(const VBO&);
    VBO& operator=(const VBO&);
};

class glBindBufferState {
public:
    glBindBufferState(GLenum target, GLuint buffer) : m_target(target) { GLCheck(glBindBuffer(m_target, buffer)); }
    ~glBindBufferState() { GLCheck(glBindBuffer(m_target, 0)); }
private:
    GLenum m_target;
};

/** Defines a vertex array object (VAO) */
class VAO {
public:
    VAO(); 
    ~VAO();

    GLuint getId() const { return m_id; }
private:
    GLuint m_id;

    VAO(const VAO&);
    VAO& operator=(const VAO&);
};

class glBindVertexArrayState {
public:
    glBindVertexArrayState(GLuint id) { GLCheck(glBindVertexArray(id)); }
    ~glBindVertexArrayState() { GLCheck(glBindVertexArray(0)); }
};

#endif
