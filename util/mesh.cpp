#include "mesh.hpp"
#include <r2tk/r2-exception.hpp>
#include <fstream>
#include <sstream>
#include <limits>
#include <vector>
#include <glm/glm.hpp>

struct GroupData {
    std::string m_material;
    std::vector<glm::vec4> m_vertices;
    std::vector<glm::vec4> m_normals;
    std::vector<glm::vec2> m_texCoords;
};

std::shared_ptr<Mesh> Mesh::loadOBJ(const std::string& filename) {
    std::shared_ptr<Mesh> mesh(new Mesh);

    std::ifstream fs(filename.c_str(), std::ifstream::in);
    if (!fs.is_open()) {
		throw r2ExceptionIOM("Failed to open .obj file: " + filename);
    }

    std::vector<glm::vec4> vertexList;
    std::vector<glm::vec4> normalList;
    std::vector<glm::vec2> texCoordList;

    std::map<std::string, GroupData> groupData;
    std::string currentGroup = "default";

    while (!fs.eof()) {
        std::string line;
        std::stringstream lineStream;

        std::getline(fs, line);
        lineStream.str(line);

        std::string token;
        lineStream >> token;

        if (token == "mtllib") {
            lineStream >> mesh->m_mtlLibrary;
        } else if (token == "g") {
            lineStream >> currentGroup;
        } else if (token == "usemtl") {
            std::string material;
            lineStream >> material;

            groupData[currentGroup].m_material = material;
        } else if (token == "v") {
            glm::vec4 v;

            lineStream >> v.x;
            lineStream >> v.y;
            lineStream >> v.z;
            v.w = 1.0f;

            if (lineStream.fail() || lineStream.bad()) {
				throw r2ExceptionIOM("Failed to parse vertex in .obj file: " + filename);
            }

            vertexList.push_back(v);
        } else if (token == "vn") {
            glm::vec4 n;

            lineStream >> n.x;
            lineStream >> n.y;
            lineStream >> n.z;
            n.w = 0.0f;

            if (lineStream.fail() || lineStream.bad()) {
				throw r2ExceptionIOM("Failed to parse normal in .obj file: " + filename);
            }

            normalList.push_back(n);
        } else if (token == "vt") {
            glm::vec2 t;
            lineStream >> t.x;
            lineStream >> t.y;

            if (lineStream.fail() || lineStream.bad()) {
				throw r2ExceptionIOM("Failed to parse texture coordinate in .obj file: " + filename);
            }

            texCoordList.push_back(t);
        } else if (token == "f") {
            for (int i = 0; i < 3; ++i) {
                int v;
                int t;
                int n;

                lineStream >> v;
                lineStream.ignore();
                lineStream >> t;
                lineStream.ignore();
                lineStream >> n;

                if (lineStream.fail() || lineStream.bad()) {
                    throw r2ExceptionIOM("Failed to parse face in .obj file: " + filename);
                }

                v = v - 1;
                n = n - 1;
                t = t - 1;

                groupData[currentGroup].m_vertices.push_back(vertexList[v]);
                groupData[currentGroup].m_normals.push_back(normalList[n]);
                groupData[currentGroup].m_texCoords.push_back(texCoordList[t]);
            }
        }
    }

    fs.close();

    // create the buffers
    for (std::map<std::string, GroupData>::iterator it = groupData.begin(); it != groupData.end(); it++)
    {
        // skip if empty group
        if (it->second.m_vertices.size() == 0)
            continue;

        // assert that we have equal amounts of vertices, texture coordinates and normals
        bool sizeCheck = it->second.m_vertices.size() == it->second.m_normals.size() &&
                         it->second.m_vertices.size() == it->second.m_texCoords.size();
        if (!sizeCheck) {
			throw r2ExceptionIOM("Invalid number of vertices/normals/texture coordinates in .obj file: " + filename);
        }

        std::shared_ptr<Mesh::Group> g(new Mesh::Group);

        {
            glBindVertexArrayState vaoState(g->m_VAO.getId());
            GLCheck(glEnableVertexAttribArray(0)); 
            GLCheck(glEnableVertexAttribArray(1));
            GLCheck(glEnableVertexAttribArray(2));

            {
                glBindBufferState vboState(GL_ARRAY_BUFFER, g->m_vertices.getId());
                GLCheck(glBufferData(GL_ARRAY_BUFFER, it->second.m_vertices.size() * 4 * sizeof(float), &it->second.m_vertices[0], GL_STATIC_DRAW));
                GLCheck(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
            }

            {
                glBindBufferState vboState(GL_ARRAY_BUFFER, g->m_normals.getId());
                GLCheck(glBufferData(GL_ARRAY_BUFFER, it->second.m_normals.size() * 4 * sizeof(float), &it->second.m_normals[0], GL_STATIC_DRAW));
                GLCheck(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0));
            }

            {
                glBindBufferState vboState(GL_ARRAY_BUFFER, g->m_texCoords.getId());
                GLCheck(glBufferData(GL_ARRAY_BUFFER, it->second.m_texCoords.size() * 2 * sizeof(float), &it->second.m_texCoords[0], GL_STATIC_DRAW));
                GLCheck(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0));
            }
        }
        
        g->m_material = it->second.m_material;
        g->m_vertexCount = it->second.m_vertices.size();
        mesh->m_groups[it->first] = g;
    }

    return mesh;
}

