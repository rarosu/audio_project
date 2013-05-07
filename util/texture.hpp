#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <memory>
#include <GL/glew.h>

class Texture {
public:
    Texture();
    ~Texture();

    GLuint getId() const { return m_id; }
    GLuint getUnitId() const { return m_unitId; }
    GLuint getSamplerId() const { return m_samplerId; }

    static std::shared_ptr<Texture> loadTexture(const std::string& filename);
private:
    static GLuint s_nextUnitId;

    GLuint m_id;
    GLuint m_unitId;
    GLuint m_samplerId;
};

#endif
