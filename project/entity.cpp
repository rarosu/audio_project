#include "entity.hpp"
#include <vector>

Entity::Entity(const std::string& objModel) {
	// load the mesh
    m_mesh = Mesh::loadOBJ("resources/meshes/crate.obj");
    m_materialLibrary = Material::loadMTL("resources/meshes/" + m_mesh->m_mtlLibrary);

	// load the shaders
    std::vector<std::shared_ptr<Shader> > shaders;

    shaders.push_back(Shader::loadShader("resources/shaders/basic.vs", GL_VERTEX_SHADER));
    shaders.push_back(Shader::loadShader("resources/shaders/basic.fs", GL_FRAGMENT_SHADER));

    m_program = std::shared_ptr<Program>(new Program(shaders));

	// load and bind the texture
    m_texture = Texture::loadTexture("resources/textures/" + m_materialLibrary[m_mesh->m_groups["default"]->m_material].m_mapKd);

    {
        glUseProgramState programBinding(m_program->getId());

        GLint samplerUniform = GLCheck(glGetUniformLocation(m_program->getId(), "Texture"));
        GLCheck(glUniform1i(samplerUniform, m_texture->getUnitId()));
        GLCheck(glActiveTexture(GL_TEXTURE0 + m_texture->getUnitId()));
        GLCheck(glBindTexture(GL_TEXTURE_2D, m_texture->getId()));

        GLCheck(glSamplerParameteri(m_texture->getSamplerId(), GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCheck(glSamplerParameteri(m_texture->getSamplerId(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCheck(glGenerateMipmap(GL_TEXTURE_2D));
        GLCheck(glSamplerParameteri(m_texture->getSamplerId(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCheck(glSamplerParameteri(m_texture->getSamplerId(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCheck(glBindSampler(m_texture->getUnitId(), m_texture->getSamplerId()));
    }
}

void Entity::setModelMatrix(const glm::mat4& modelMatrix) {
	m_modelMatrix = modelMatrix;
}

void Entity::render(const Camera& camera, const PointLight& pointLight, const glm::vec3& ambientLightIntensity) {
	glUseProgramState programBinding(m_program->getId());

	// activate the texture unit, bind the texture and bind the sampler
	GLCheck(glActiveTexture(GL_TEXTURE0 + m_texture->getUnitId()));
	GLCheck(glBindTexture(GL_TEXTURE_2D, m_texture->getId()));
	GLCheck(glBindSampler(m_texture->getUnitId(), m_texture->getSamplerId()));

	// create the matrices we need
	const glm::mat4& view = camera.getView();
    const glm::mat4& projection = camera.getProjection();

	glm::mat4 viewWorld = view * m_modelMatrix;
	glm::mat3 normalViewWorld = glm::transpose(glm::inverse(glm::mat3(viewWorld)));
	glm::mat3 normalView = glm::transpose(glm::inverse(glm::mat3(view)));

	// pass the matrices to the shader
    GLint projectionUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_Projection"));
    GLint viewWorldUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_ViewWorld"));
    GLint normalViewWorldUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_NormalViewWorld"));
        
    GLCheck(glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, &projection[0][0]));
    GLCheck(glUniformMatrix4fv(viewWorldUniform, 1, GL_FALSE, &viewWorld[0][0]));
    GLCheck(glUniformMatrix3fv(normalViewWorldUniform, 1, GL_FALSE, &normalViewWorld[0][0]));

	// transform the point light position to view space and pass the light attributes to the shader
	glm::vec4 lightPositionV = view * pointLight.m_position;

	GLint ambientIntensityUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_AmbientIntensity"));
    GLint directionalIntensityUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_LightIntensity"));
	GLint lightPositionUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_LightPositionV"));

	GLCheck(glUniform3fv(ambientIntensityUniform, 1, &ambientLightIntensity[0]));
	GLCheck(glUniform3fv(directionalIntensityUniform, 1, &pointLight.m_intensity[0]));
	GLCheck(glUniform4fv(lightPositionUniform, 1, &lightPositionV[0]));

	// render the mesh
    glBindVertexArrayState vaoBinding(m_mesh->m_groups["default"]->m_VAO.getId());
    GLCheck(glDrawArrays(GL_TRIANGLES, 0, m_mesh->m_groups["default"]->m_vertexCount));
}