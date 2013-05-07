#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <map>
#include <string>
#include <glm/glm.hpp>

struct Material {
public:
    Material() : m_Ns(0) {}

    glm::vec3 m_Ka;         // Ambient reflectivity
    glm::vec3 m_Kd;         // Diffuse reflectivity
    glm::vec3 m_Ks;         // Specular reflectivity
    glm::vec3 m_Tf;         // Transmission filter
    float m_Ns;             // Specular exponent

    std::string m_mapKa;    // Ambient texture
    std::string m_mapKd;    // Diffuse texture
    std::string m_mapKs;    // Specular texture
    std::string m_mapNs;    // Specular exponent texture

    static std::map<std::string, Material> loadMTL(const std::string& libraryFile);
};

#endif
