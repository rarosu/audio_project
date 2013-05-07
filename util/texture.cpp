#include "texture.hpp"
#include "utility.hpp"
#include <IL/il.h>

GLuint Texture::s_nextUnitId = 1;

Texture::Texture() {
    GLCheck(glGenTextures(1, &m_id));
    GLCheck(glGenSamplers(1, &m_samplerId));
    m_unitId = s_nextUnitId++;
}

Texture::~Texture() {
    GLCheck(glDeleteTextures(1, &m_id));
    GLCheck(glDeleteSamplers(1, &m_samplerId));
}

std::shared_ptr<Texture> Texture::loadTexture(const std::string& filename) {
    std::shared_ptr<Texture> texture(new Texture);
    
    ILuint imageName;
    ilGenImages(1, &imageName);
    ilBindImage(imageName);
    if (!ilLoadImage(filename.c_str())) {
        std::cerr << "ERROR: Failed to load texture image: " << filename << std::endl;
        return std::shared_ptr<Texture>();
    }

    GLCheck(glBindTexture(GL_TEXTURE_2D, texture->getId()));

    ILuint imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
    ILuint imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);
    ILuint imageBpp = ilGetInteger(IL_IMAGE_BPP);
    ILubyte* imageData = ilGetData();
    GLenum format = (imageBpp == 3) ? GL_RGB : GL_RGBA;

    if (imageBpp != 3 && imageBpp != 4) {
        std::cerr << "ERROR: Texture image in bad format (bytes per pixel = " << imageBpp << "): " << filename << std::endl;
        return std::shared_ptr<Texture>();
    }

    GLCheck(glTexImage2D(GL_TEXTURE_2D,
                0,
                format,
                imageWidth,
                imageHeight,
                0,
                format,
                GL_UNSIGNED_BYTE,
                (const GLvoid*)imageData));

    ilBindImage(0);
    ilDeleteImages(1, &imageName);

    return texture;
}
