#include "buffer.hpp"

VBO::VBO() {
    GLCheck(glGenBuffers(1, &m_id));
}

VBO::~VBO() {
    GLCheck(glDeleteBuffers(1, &m_id));
}


VAO::VAO() {
    GLCheck(glGenVertexArrays(1, &m_id));
}

VAO::~VAO() {
    GLCheck(glDeleteVertexArrays(1, &m_id));
}

