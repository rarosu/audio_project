#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <memory>
#include <map>
#include <util\util.hpp>

/** Holds the data describing a point light */
struct PointLight {
	glm::vec4 m_position;
	glm::vec3 m_intensity;
};

/** Manages an entity in the 3D scene */
class Entity {
public:
	Entity(const std::string& objModel);

	void setModelMatrix(const glm::mat4& modelMatrix);
	void render(const Camera& camera, const PointLight& pointLight, const glm::vec3& ambientLightIntensity);
private:
	std::shared_ptr<Program> m_program;
	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<Mesh> m_mesh;
	std::map<std::string, Material> m_materialLibrary;

	glm::mat4 m_modelMatrix;
};

#endif