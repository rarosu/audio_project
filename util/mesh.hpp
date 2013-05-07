#ifndef MESH_HPP
#define MESH_HPP

#include <string>
#include <memory>
#include <map>
#include "buffer.hpp"

class Mesh {
public:
    class Group {
    public:
        std::string m_material;

        VBO m_vertices;
        VBO m_normals;
        VBO m_texCoords;
        size_t m_vertexCount;

        VAO m_VAO;
    };

    std::string m_mtlLibrary;
    std::map<std::string, std::shared_ptr<Group> > m_groups;

    static std::shared_ptr<Mesh> loadOBJ(const std::string& filename);
};

#endif
