#include <util/util.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <iostream>
#include <string>
#include <memory>

class Lab : public LabTemplate {
public:
    Lab();
	~Lab();

    void onUpdate(float dt, const InputState& currentInput, const InputState& previousInput);
    void onRender(float dt, float interpolation);
    void onResize(int width, int height);
private:
	ALuint m_backgroundBuffer;
	ALuint m_backgroundSource;

    std::shared_ptr<Program> m_program;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Mesh> m_mesh;
    std::map<std::string, Material> m_materialLibrary;

    float m_cameraPolarAngle;
    Camera m_camera;
};

int main(int argc, char* argv[]) {
    try {
        LabApplication application;

        glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);

        LabApplication::ContextDescription description;
		description.m_openglVersionMajor = 4;
		description.m_openglVersionMinor = 0;
        description.m_windowWidth = 800;
        description.m_windowHeight = 600;
        description.m_windowTitle = "Texturing & Lighting";
        application.createContext(description);

        std::unique_ptr<LabTemplate> lab(new Lab);
        application.start(lab);
    } catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

Lab::Lab()
    : m_cameraPolarAngle(0.0f) {
    // set state
    GLCheck(glEnable(GL_DEPTH_TEST));
    GLCheck(glDepthFunc(GL_LESS));
    GLCheck(glEnable(GL_CULL_FACE));
    GLCheck(glCullFace(GL_BACK));
    GLCheck(glFrontFace(GL_CCW));

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
        //GLCheck(glSamplerParameteri(m_texture->getSamplerId(), GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        //GLCheck(glSamplerParameteri(m_texture->getSamplerId(), GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCheck(glSamplerParameteri(m_texture->getSamplerId(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCheck(glSamplerParameteri(m_texture->getSamplerId(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCheck(glBindSampler(m_texture->getUnitId(), m_texture->getSamplerId()));
    }

    // setup the camera
    m_camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
    m_camera.setFacing(glm::vec3(0.0f, 0.0f, -1.0f));
    m_camera.setPosition(glm::vec3(0.0f, 0.0f, 25.0f));

    // setup a directional light
    {
        glUseProgramState programBinding(m_program->getId());

        GLint intensityUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_LightIntensity"));

        GLCheck(glUniform4f(intensityUniform, 1.0f, 1.0f, 1.0f, 1.0f));
    }

	if (!alutInit(NULL, NULL)) {
		std::cerr << "Failed to initialize OpenAL" << std::endl;
	}

	ALenum alError;

	// setup OpenAL buffers
	alGenBuffers(1, &m_backgroundBuffer);
	if ((alError = alGetError()) != AL_NO_ERROR) {
		throw r2ExceptionRuntimeM("Failed to generate sound buffer");
	}

	// load the wav
	
	// set the buffer data
	//alBufferData(m_backgroundBuffer

	// unload the wav

	// generate a source depending on the buffer
	//alGenSources(1, &m_backgroundSource);
	//alSourcei(m_backgroundSource, AL_BUFFER, m_backgroundBuffer);

}

Lab::~Lab() {
	//alDeleteSources(1, &m_backgroundSource);
	//alDeleteBuffers(1, &m_backgroundBuffer);

	alutExit();
}

void Lab::onUpdate(float dt, const InputState& currentInput, const InputState& previousInput) {
    if (currentInput.m_keyboard.m_keys[GLFW_KEY_RIGHT])
        m_cameraPolarAngle += M_PI * 0.5 * dt;
    if (currentInput.m_keyboard.m_keys[GLFW_KEY_LEFT])
        m_cameraPolarAngle -= M_PI * 0.5 * dt;

    glm::vec3 cameraPosition;
    cameraPosition.x = 25.0f * cos(m_cameraPolarAngle);
    cameraPosition.y = 10.0f;
    cameraPosition.z = -25.0f * sin(m_cameraPolarAngle);

    glm::vec3 cameraOrientation = -cameraPosition;

    m_camera.setPosition(cameraPosition);
    m_camera.setFacing(cameraOrientation);
    m_camera.commit();
}

void Lab::onRender(float dt, float interpolation) {
    GLCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    {
        glUseProgramState programBinding(m_program->getId());
        
        const glm::mat4& view = m_camera.getView();
        const glm::mat4& projection = m_camera.getProjection();

        // pass the matrices to the shader
        GLint projectionUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_Projection"));
        GLint viewWorldUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_ViewWorld"));
        GLint normalViewWorldUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_NormalViewWorld"));
        
        GLCheck(glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, &projection[0][0]));
        GLCheck(glUniformMatrix4fv(viewWorldUniform, 1, GL_FALSE, &view[0][0]));
        GLCheck(glUniformMatrix4fv(normalViewWorldUniform, 1, GL_FALSE, &view[0][0]));

        // transform the light direction
        glm::vec4 lightDirection(0.5f, 0.0f, -1.0f, 0.0f);
        lightDirection = view * lightDirection;
        lightDirection = -glm::normalize(lightDirection);

        GLint lightDirectionUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_LightDirection"));
        GLCheck(glUniform4fv(lightDirectionUniform, 1, &lightDirection[0]));

        // render the mesh
        glBindVertexArrayState vaoBinding(m_mesh->m_groups["default"]->m_VAO.getId());
        GLCheck(glDrawArrays(GL_TRIANGLES, 0, m_mesh->m_groups["default"]->m_vertexCount));
    }

    glfwSwapBuffers();
}

void Lab::onResize(int width, int height) {
    m_camera.setProjection(Camera::createPerspectiveProjection(1.0f, 100.0f, M_PI * 0.25f, (float)width / height));
}

