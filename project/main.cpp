#include <util/util.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <r2tk\r2-data-types.hpp>
#include "sound.hpp"
#include "entity.hpp"

class Lab : public LabTemplate {
public:
    Lab();
	~Lab();

    void onUpdate(float dt, const InputState& currentInput, const InputState& previousInput);
    void onRender(float dt, float interpolation);
    void onResize(int width, int height);
private:
	std::shared_ptr<WAVHandle> m_sound;
	std::shared_ptr<SoundSource> m_source;

	PointLight m_pointLight;
	glm::vec3 m_ambientLight;

	std::shared_ptr<Entity> m_boxEntity;
	float m_boxModelOrientation;
	glm::mat4 m_boxModelMatrix;

	float m_cameraOrientation;
    glm::vec3 m_cameraPosition;
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
		std::cin.get();
        return 1;
    }
    
    return 0;
}


Lab::Lab()
    : m_cameraOrientation(-M_PI * 0.5f)
	, m_cameraPosition(0.0f, 0.0f, 10.0f)
	, m_boxModelOrientation(0.0f) {

    // set state
    GLCheck(glEnable(GL_DEPTH_TEST));
    GLCheck(glDepthFunc(GL_LESS));
    GLCheck(glEnable(GL_CULL_FACE));
    GLCheck(glCullFace(GL_BACK));
    GLCheck(glFrontFace(GL_CCW));

	// load entities
	m_boxEntity = std::shared_ptr<Entity>(new Entity("resources/meshes/crate.obj"));

	// setup the lights
	m_pointLight.m_intensity = glm::vec3(0.8, 0.8, 0.8);
	m_pointLight.m_position = glm::vec4(0.0f, 3.0, 6.0, 1.0);
	m_ambientLight = glm::vec3(0.2, 0.2, 0.2);

    // setup the camera
    m_camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
    m_camera.setFacing(glm::vec3(0.0f, 0.0f, -1.0f));
    m_camera.setPosition(glm::vec3(0.0f, 0.0f, 25.0f));

	// initialize OpenAL
	if (!alutInit(NULL, NULL)) {
		std::cerr << "Failed to initialize OpenAL" << std::endl;
	}

	m_sound = std::shared_ptr<WAVHandle>(new WAVHandle("resources/sounds/wind-howl-01.wav"));
	m_source = std::shared_ptr<SoundSource>(new SoundSource(m_sound, glm::vec3(0.0f, 0.0f, 0.0f), false));
	m_source->play();
}

Lab::~Lab() {	
	// Whale, whale, whale, what have we here?
	//
	//    ___\|/__
	//   /^  >    \/|
	//   \-_______/\|
	//	  
	alutExit();
}

void Lab::onUpdate(float dt, const InputState& currentInput, const InputState& previousInput) {
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_RIGHT] || currentInput.m_keyboard.m_keys['D'])
		m_cameraOrientation += M_PI * dt;
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_LEFT] || currentInput.m_keyboard.m_keys['A'])
		m_cameraOrientation -= M_PI * dt;

	glm::vec3 cameraOrientation;
	cameraOrientation.x = cos(m_cameraOrientation);
	cameraOrientation.y = 0.0f;
	cameraOrientation.z = sin(m_cameraOrientation);

	if (currentInput.m_keyboard.m_keys[GLFW_KEY_UP] || currentInput.m_keyboard.m_keys['W'])
		m_cameraPosition += cameraOrientation * 10.0f * dt;
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_DOWN] || currentInput.m_keyboard.m_keys['S'])
		m_cameraPosition -= cameraOrientation * 10.0f * dt;
		

	glm::vec3 rightOrientation = glm::cross(cameraOrientation, glm::vec3(0.0f, 1.0f, 0.0f));
	if (currentInput.m_keyboard.m_keys['E'])
		m_cameraPosition += rightOrientation * 10.0f * dt;
	if (currentInput.m_keyboard.m_keys['Q'])
		m_cameraPosition -= rightOrientation * 10.0f * dt;

	m_camera.setPosition(m_cameraPosition);
	m_camera.setFacing(cameraOrientation);
	m_camera.commit();

	// set the OpenAL listener by the camera
	alListener3f(AL_POSITION, m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z);

	// rotate the box at a constant speed
	m_boxModelOrientation += M_PI * 0.1f * dt;
	m_boxModelMatrix = glm::mat4(cos(m_boxModelOrientation),  0, sin(m_boxModelOrientation), 0,
								 0,						   1, 0,						     0,
								 -sin(m_boxModelOrientation), 0, cos(m_boxModelOrientation), 0,
								 0,						   0, 0,						     1);
	m_boxEntity->setModelMatrix(m_boxModelMatrix);

	// update sound source
	m_source->update();
}

void Lab::onRender(float dt, float interpolation) {
    GLCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_boxEntity->render(m_camera, m_pointLight, m_ambientLight);

    glfwSwapBuffers();
}

void Lab::onResize(int width, int height) {
    m_camera.setProjection(Camera::createPerspectiveProjection(1.0f, 100.0f, M_PI * 0.25f, (float)width / height));
}

