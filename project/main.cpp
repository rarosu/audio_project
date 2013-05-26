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
#include <fftw3.h>

const int SOURCE_COUNT = 4;

class Lab : public LabTemplate {
public:
    Lab();
	~Lab();

    void onUpdate(float dt, const InputState& currentInput, const InputState& previousInput);
    void onRender(float dt, float interpolation);
    void onResize(int width, int height);
private:
	// Sound
	Listener m_listener;
	std::vector<std::shared_ptr<WAVHandle> > m_sounds;
	std::vector<std::shared_ptr<SoundSource> > m_sources;

	// Light
	PointLight m_pointLight;
	glm::vec3 m_ambientLight;

	// Entities
	std::shared_ptr<Entity> m_planeEntity;

	std::vector<std::shared_ptr<Entity> > m_entities;
	std::vector<glm::vec3> m_translations;
	float m_orientation;

	// Camera
	float m_cameraOrientation;
    glm::vec3 m_cameraPosition;
    Camera m_camera;


	// Helper methods
	glm::vec3 getCameraOrientation(float orientation) const;
	glm::mat4 createModelMatrix(float orientation, const glm::vec3& translation);
};

int main(int argc, char* argv[]) {
    try {
		// Start up the lab, see the Lab methods for implementation
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
	, m_orientation(0.0f)
	, m_sounds(SOURCE_COUNT)
	, m_sources(SOURCE_COUNT) {

    // set state
    GLCheck(glEnable(GL_DEPTH_TEST));
    GLCheck(glDepthFunc(GL_LESS));
    GLCheck(glEnable(GL_CULL_FACE));
    GLCheck(glCullFace(GL_BACK));
    GLCheck(glFrontFace(GL_CCW));

	// load entities
	m_translations.push_back(glm::vec3(0, 0, 0));
	m_translations.push_back(glm::vec3(30, 0, 0));
	m_translations.push_back(glm::vec3(-30, 0, 0));

	for (int i = 0; i < 3; ++i) {
		m_entities.push_back(std::shared_ptr<Entity>(new Entity("resources/meshes/crate.obj")));
		m_entities.back()->setModelMatrix(createModelMatrix(m_orientation, m_translations[i]));
	}
	

	glm::mat4 planeModelMatrix = glm::mat4(1, 0, 0, 0,
								   0, 1, 0, 0,
								   0, 0, 1, 0,
								   0, -3, 0, 1);
	m_planeEntity = std::shared_ptr<Entity>(new Entity("resources/meshes/cobblestone-plane.obj"));
	m_planeEntity->setModelMatrix(planeModelMatrix);

	// setup the lights
	m_pointLight.m_intensity = glm::vec3(0.8, 0.8, 0.8);
	m_pointLight.m_position = glm::vec4(0.0f, 3.0, 6.0, 1.0);
	m_ambientLight = glm::vec3(0.2, 0.2, 0.2);

    // setup the camera
    m_camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
	m_camera.setFacing(getCameraOrientation(m_cameraOrientation));
    m_camera.setPosition(glm::vec3(0.0f, 0.0f, 25.0f));

	// initialize OpenAL
	if (!alutInit(NULL, NULL)) {
		std::cerr << "Failed to initialize OpenAL" << std::endl;
	}

	m_listener.m_position = m_cameraPosition;
	m_listener.m_facing = getCameraOrientation(m_cameraOrientation);

	// WARNING:
	// Initial filters put on all sources. Note: Neither LowpassFilter or ConvolutionFilter works as intended.
	// Uncomment at your own risk!
	std::vector<std::shared_ptr<Filter> > filters;
	//filters.push_back(std::shared_ptr<Filter>(new LowpassFilter(1000, 44100)));
	//filters.push_back(std::shared_ptr<Filter>(new ConvolutionFilter(ConvolutionFilter::generateSquareImpulse(80, 0.6))));

	m_sounds[0] = std::shared_ptr<WAVHandle>(new WAVHandle("resources/sounds/Kalimba.wav"));
	m_sounds[1] = std::shared_ptr<WAVHandle>(new WAVHandle("resources/sounds/car-alarm-1.wav"));
	m_sounds[2] = std::shared_ptr<WAVHandle>(new WAVHandle("resources/sounds/Maid with the Flaxen Hair.wav"));
	m_sounds[3] = std::shared_ptr<WAVHandle>(new WAVHandle("resources/sounds/Sleep Away.wav"));

	m_sources[0] = std::shared_ptr<SoundSource>(new SoundSource(m_sounds[0], glm::vec3(0.0f, 0.0f, 0.0f), true, false, m_listener, filters));
	m_sources[1] = std::shared_ptr<SoundSource>(new SoundSource(m_sounds[1], m_translations[0], true, true, m_listener, filters));
	m_sources[2] = std::shared_ptr<SoundSource>(new SoundSource(m_sounds[2], m_translations[1], true, true, m_listener, filters));
	m_sources[3] = std::shared_ptr<SoundSource>(new SoundSource(m_sounds[3], m_translations[2], true, true, m_listener, filters));

	m_sources[0]->addFilter(std::shared_ptr<Filter>(new VolumeFilter(0.2f)));
	m_sources[1]->addFilter(std::shared_ptr<Filter>(new VolumeFilter(0.2f)));

	
	// Start sounds as stopped

	//for (int i = 0; i < m_sources.size(); ++i) {
	//	m_sources[i]->play();
	//}
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
	// Keys for toggling sounds on/off
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_KP_1] && !previousInput.m_keyboard.m_keys[GLFW_KEY_KP_1])
		m_sources[0]->toggle();
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_KP_2] && !previousInput.m_keyboard.m_keys[GLFW_KEY_KP_2])
		m_sources[1]->toggle();
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_KP_3] && !previousInput.m_keyboard.m_keys[GLFW_KEY_KP_3])
		m_sources[2]->toggle();
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_KP_4] && !previousInput.m_keyboard.m_keys[GLFW_KEY_KP_4])
		m_sources[3]->toggle();


	// Keys for controlling the camera
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_RIGHT] || currentInput.m_keyboard.m_keys['D'])
		m_cameraOrientation += M_PI * dt;
	if (currentInput.m_keyboard.m_keys[GLFW_KEY_LEFT] || currentInput.m_keyboard.m_keys['A'])
		m_cameraOrientation -= M_PI * dt;

	glm::vec3 cameraOrientation = getCameraOrientation(m_cameraOrientation);

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

	// set the listener by the camera
	m_listener.m_position = m_cameraPosition;
	m_listener.m_facing = cameraOrientation;

	// rotate the boxes at a constant speed
	m_orientation += M_PI * 0.1f * dt;
	for (int i = 0; i < m_entities.size(); ++i) {
		m_entities[i]->setModelMatrix(createModelMatrix(m_orientation, m_translations[i]));
	}

	// update sound sources
	for (int i = 0; i < m_sources.size(); ++i) {
		m_sources[i]->update();
	}
}

void Lab::onRender(float dt, float interpolation) {
    GLCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_planeEntity->render(m_camera, m_pointLight, m_ambientLight);
	for (int i = 0; i < m_entities.size(); ++i) {
		m_entities[i]->render(m_camera, m_pointLight, m_ambientLight);
	}

    glfwSwapBuffers();
}

void Lab::onResize(int width, int height) {
    m_camera.setProjection(Camera::createPerspectiveProjection(1.0f, 100.0f, M_PI * 0.25f, (float)width / height));
}



glm::vec3 Lab::getCameraOrientation(float orientation) const {
	glm::vec3 cameraOrientation;
	cameraOrientation.x = cos(m_cameraOrientation);
	cameraOrientation.y = 0.0f;
	cameraOrientation.z = sin(m_cameraOrientation);

	return cameraOrientation;
}

glm::mat4 Lab::createModelMatrix(float orientation, const glm::vec3& translation) {
	return			   glm::mat4(cos(orientation),  0, sin(orientation), 0,
								 0,					1, 0,				 0,
								 -sin(orientation), 0, cos(orientation), 0,
								 translation.x, translation.y, translation.z, 1);
}
