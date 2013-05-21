#include "material.hpp"
#include <r2tk/r2-exception.hpp>
#include <fstream>
#include <sstream>

std::map<std::string, Material> Material::loadMTL(const std::string& libraryFile) {
    std::ifstream fs(libraryFile.c_str(), std::ifstream::in);
    if (!fs.is_open()) {
        throw r2ExceptionIOM("Failed to open .mtl file: " + libraryFile);
    }

    
    std::map<std::string, Material> materials;
    std::string currentName;

    while (!fs.eof()) {
        std::string line;
        std::stringstream lineStream;

        std::getline(fs, line);
        lineStream.str(line);

        std::string token;
        lineStream >> token; 

        if (token == "newmtl") {
            lineStream >> currentName;
        } else if (token == "Ka") {
            lineStream >> materials[currentName].m_Ka.r;
            lineStream >> materials[currentName].m_Ka.g;
            lineStream >> materials[currentName].m_Ka.b;
        } else if (token == "Kd") {
            lineStream >> materials[currentName].m_Kd.r;
            lineStream >> materials[currentName].m_Kd.g;
            lineStream >> materials[currentName].m_Kd.b;
        } else if (token == "Ks") {
            lineStream >> materials[currentName].m_Ks.r;
            lineStream >> materials[currentName].m_Ks.g;
            lineStream >> materials[currentName].m_Ks.b;
        } else if (token == "Tf") {
            lineStream >> materials[currentName].m_Tf.r;
            lineStream >> materials[currentName].m_Tf.g;
            lineStream >> materials[currentName].m_Tf.b;
        } else if (token == "Ns") {
            lineStream >> materials[currentName].m_Ns;
        } else if (token == "map_Ka") {
            lineStream >> materials[currentName].m_mapKa;
        } else if (token == "map_Kd") {
            lineStream >> materials[currentName].m_mapKd;
        } else if (token == "map_Ks") {
            lineStream >> materials[currentName].m_mapKs;
        } else if (token == "map_Ns") {
            lineStream >> materials[currentName].m_mapNs;
        }
    }
    
    return materials;
}
