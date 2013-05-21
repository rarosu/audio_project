#include <util/util.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <r2tk\r2-data-types.hpp>

const int SOUND_CHUNK_COUNT = 2;
const int SOUND_CHUNK_SIZE = 44100 * 3;

struct WAVHandle {
	std::ifstream m_file;
	unsigned int m_channelCount;
	unsigned int m_sampleRate;
	unsigned int m_bytesPerSample;
	unsigned int m_dataSize;

	WAVHandle(const std::string& filepath);
	~WAVHandle() throw();
	
	std::streamsize readChunk(size_t chunkSize, unsigned char* data);
	ALenum getFormat() const;
};

class Lab : public LabTemplate {
public:
    Lab();
	~Lab();

    void onUpdate(float dt, const InputState& currentInput, const InputState& previousInput);
    void onRender(float dt, float interpolation);
    void onResize(int width, int height);
private:
	ALuint m_backgroundSource;
	std::vector<ALuint> m_backgroundBuffers;
	std::shared_ptr<WAVHandle> m_soundFileHandle;

    std::shared_ptr<Program> m_program;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Mesh> m_mesh;
    std::map<std::string, Material> m_materialLibrary;

	float m_modelOrientation;
	glm::mat4 m_modelMatrix;

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
		std::cin.get();
        return 1;
    }
    
    return 0;
}

WAVHandle::WAVHandle(const std::string& filepath) {
	
	// read the file
	m_file.open(filepath.c_str(), std::ios::binary);

	if (!m_file.is_open())
		throw r2ExceptionIOM("Failed to open wav file: " + filepath);

	// check the RIFF header
	char riffHeader[12];
	m_file.read(riffHeader, 12);

	if (strncmp((const char*)&riffHeader[0], "RIFF", 4) != 0)
		throw r2ExceptionIOM("Failed to read .wav file: " + filepath + " (not a RIFF file)");
	if (strncmp((const char*)&riffHeader[8], "WAVE", 4) != 0)
		throw r2ExceptionIOM("Failed to read .wav file: " + filepath + " (not a WAVE file)");

	std::cout << sizeof(unsigned int) << std::endl;

	char subchunkHeader[8];
	do {
		m_file.read(subchunkHeader, 8);
		if (strncmp((const char*)&subchunkHeader[0], "fmt", 3) == 0) {
			char fmtHeader[16];
			m_file.read(fmtHeader, 16);

			m_channelCount = *(unsigned short*) &fmtHeader[2];
			m_sampleRate = *(unsigned int*) &fmtHeader[4];
			m_bytesPerSample = (*(unsigned int*) &fmtHeader[8]) / m_sampleRate;
		} else if (strncmp((const char*)&subchunkHeader[0], "data", 4) == 0) {
			m_dataSize = *(unsigned int*) &subchunkHeader[4];
			break;
		} else {
			unsigned int fnulSize = (*(unsigned int*)&subchunkHeader[4]);
			char* fnul = new char[fnulSize];
			m_file.read(fnul, fnulSize);
			delete[] fnul;
		}
	} while (!m_file.eof()); 
}

WAVHandle::~WAVHandle() throw() {
	m_file.close();
}
	
std::streamsize WAVHandle::readChunk(size_t chunkSize, unsigned char* data) {
	m_file.read((char*)data, chunkSize);
	return m_file.gcount();
}

ALenum WAVHandle::getFormat() const {
	char field = ((m_channelCount == 2) << 1) | 
				 (m_bytesPerSample == 4);

	ALenum format;
	switch (field) {
	case 0:
		format = AL_FORMAT_MONO8;
		break;
	case 1:
		format = AL_FORMAT_MONO16;
		break;
	case 2:
		format = AL_FORMAT_STEREO8;
		break;
	case 3:
		format = AL_FORMAT_STEREO16;
		break;
	}

	return format;
}


Lab::Lab()
    : m_cameraPolarAngle(0.0f)
	, m_modelOrientation(0.0f) {
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

    // setup ambient and point light
    {
        glUseProgramState programBinding(m_program->getId());

		GLint ambientIntensityUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_AmbientIntensity"));
        GLint directionalIntensityUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_LightIntensity"));

		GLCheck(glUniform3f(ambientIntensityUniform, 0.2f, 0.2f, 0.2f));
        GLCheck(glUniform3f(directionalIntensityUniform, 0.8f, 0.8f, 0.8f));
    }


	// initialize OpenAL
	if (!alutInit(NULL, NULL)) {
		std::cerr << "Failed to initialize OpenAL" << std::endl;
	}

	// setup OpenAL buffers
	m_backgroundBuffers.resize(SOUND_CHUNK_COUNT);

	m_soundFileHandle = std::shared_ptr<WAVHandle>(new WAVHandle("resources/sounds/wind-howl-01.wav"));

	alGenBuffers(SOUND_CHUNK_COUNT, &m_backgroundBuffers[0]);
	if (alGetError() != AL_NO_ERROR)
		throw r2ExceptionRuntimeM("Failed to generate buffers");
	
	for (int i = 0; i < SOUND_CHUNK_COUNT; ++i) {
		std::vector<unsigned char> bufferData(SOUND_CHUNK_SIZE);
		std::streamsize bytesRead = m_soundFileHandle->readChunk(SOUND_CHUNK_SIZE, &bufferData[0]);
		if (bytesRead != SOUND_CHUNK_SIZE) {
			// TODO: Whale, whale, whale, what have we here?
			//
			//    ___\|/__
			//   /^  >    \/|
			//   \-_______/\|
			//	  
		}

		alBufferData(m_backgroundBuffers[i],
			m_soundFileHandle->getFormat(),
			&bufferData[0],
			bytesRead,
			m_soundFileHandle->m_sampleRate);
		if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to send data to buffer");
	}
	
	
	// generate a source depending on the buffer
	alGenSources(1, &m_backgroundSource);
	if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to generate source");

	alSourceQueueBuffers(m_backgroundSource, SOUND_CHUNK_COUNT, &m_backgroundBuffers[0]);
	if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to queue buffers to source");

	alSourcePlay(m_backgroundSource);
	if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to play source");
}

Lab::~Lab() {
	alSourceStop(m_backgroundSource);

	alDeleteSources(1, &m_backgroundSource);
	alDeleteBuffers(SOUND_CHUNK_COUNT, &m_backgroundBuffers[0]);
	
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

	// rotate the box at a constant speed
	m_modelOrientation += M_PI * 0.1f * dt;
	m_modelMatrix = glm::mat4(cos(m_modelOrientation),  0, sin(m_modelOrientation), 0,
							  0,						1, 0,						0,
							  -sin(m_modelOrientation), 0, cos(m_modelOrientation), 0,
							  0,						0, 0,						1);

	// set the OpenAL listener by the camera
	alListener3f(AL_POSITION, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// process the source
	int processed = 0;
	alGetSourcei(m_backgroundSource, AL_BUFFERS_PROCESSED, &processed);

	while (processed--) {
		ALuint buffer;

		alSourceUnqueueBuffers(m_backgroundSource, 1, &buffer);
		if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Goto hell");

		std::cout << "Processed buffer " << buffer << std::endl;

		std::vector<unsigned char> bufferData(SOUND_CHUNK_SIZE);
		m_soundFileHandle->readChunk(SOUND_CHUNK_SIZE, &bufferData[0]);

		alBufferData(buffer, m_soundFileHandle->getFormat(), &bufferData[0], SOUND_CHUNK_SIZE, m_soundFileHandle->m_sampleRate);

		alSourceQueueBuffers(m_backgroundSource, 1, &buffer);
	}
}

void Lab::onRender(float dt, float interpolation) {
    GLCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    {
        glUseProgramState programBinding(m_program->getId());
        
        const glm::mat4& view = m_camera.getView();
        const glm::mat4& projection = m_camera.getProjection();

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

        // transform the point light position to view space
        glm::vec4 lightPosition(0.0f, 2.0f, -2.0f, 1.0f);
        lightPosition = view * lightPosition;

        GLint lightPositionUniform = GLCheck(glGetUniformLocation(m_program->getId(), "g_LightPositionV"));
        GLCheck(glUniform4fv(lightPositionUniform, 1, &lightPosition[0]));

        // render the mesh
        glBindVertexArrayState vaoBinding(m_mesh->m_groups["default"]->m_VAO.getId());
        GLCheck(glDrawArrays(GL_TRIANGLES, 0, m_mesh->m_groups["default"]->m_vertexCount));
	}

    glfwSwapBuffers();
}

void Lab::onResize(int width, int height) {
    m_camera.setProjection(Camera::createPerspectiveProjection(1.0f, 100.0f, M_PI * 0.25f, (float)width / height));
}

