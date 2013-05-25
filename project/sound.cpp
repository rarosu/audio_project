#include "sound.hpp"
#include <iostream>
#include <fstream>
#include <r2tk\r2-exception.hpp>

glm::vec3 Listener::getRight() const {
	return glm::cross(m_facing, glm::vec3(0, 1, 0));
}

glm::vec3 Listener::getLeft() const {
	return -getRight();
}

WAVHandle::WAVHandle(const std::string& filepath) {
	// read the file
	std::ifstream file(filepath.c_str(), std::ios::binary);

	if (!file.is_open())
		throw r2ExceptionIOM("Failed to open wav file: " + filepath);

	// check the RIFF header
	char riffHeader[12];
	file.read(riffHeader, 12);

	if (strncmp((const char*)&riffHeader[0], "RIFF", 4) != 0)
		throw r2ExceptionIOM("Failed to read .wav file: " + filepath + " (not a RIFF file)");
	if (strncmp((const char*)&riffHeader[8], "WAVE", 4) != 0)
		throw r2ExceptionIOM("Failed to read .wav file: " + filepath + " (not a WAVE file)");

	// Check subchunk headers
	char subchunkHeader[8];
	do {
		file.read(subchunkHeader, 8);
		if (strncmp((const char*)&subchunkHeader[0], "fmt", 3) == 0) {
			// Read format header
			char fmtHeader[16];
			file.read(fmtHeader, 16);

			m_channelCount = *(unsigned short*) &fmtHeader[2];
			m_sampleRate = *(unsigned int*) &fmtHeader[4];
			m_bytesPerSample = (*(unsigned int*) &fmtHeader[8]) / m_sampleRate;
		} else if (strncmp((const char*)&subchunkHeader[0], "data", 4) == 0) {
			// Read data header
			unsigned int dataSize = *(unsigned int*) &subchunkHeader[4];
			
			m_data.resize(dataSize);
			file.read((char*)&m_data[0], dataSize);

			break;
		} else {
			// Discard other headers
			unsigned int fnulSize = (*(unsigned int*)&subchunkHeader[4]);
			char* fnul = new char[fnulSize];
			file.read(fnul, fnulSize);
			delete[] fnul;
		}
	} while (!file.eof());
}

ALenum WAVHandle::getFormat() const {
	char bitfield = ((m_channelCount == 2) << 1) | 
				    (m_bytesPerSample == 4);

	ALenum format;
	switch (bitfield) {
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

std::vector<unsigned char> WAVHandle::getChunk(unsigned int streamPosition, unsigned int chunkSize) const {
	int remaining = m_data.size() - streamPosition;

	return std::vector<unsigned char>(m_data.begin() + streamPosition,
									  (remaining > chunkSize) ? m_data.begin() + streamPosition + chunkSize : m_data.end());
}

//int WAVHandle::getChunk(unsigned int streamPosition, unsigned int chunkSize, unsigned char* data) {
//	int remaining = m_data.size() - streamPosition;
//	remaining = (remaining >= 0) ? remaining : 0;
//	int bytesRead = (remaining < chunkSize) ? remaining : chunkSize;
//	
//	memcpy(data, &m_data[streamPosition], bytesRead);
//
//	return bytesRead;
//}


SoundBuffer::SoundBuffer() {
	alGenBuffers(1, &m_id);
}

SoundBuffer::~SoundBuffer() throw() {
	alDeleteBuffers(1, &m_id);
}


const int SoundSource::CHUNK_SIZE = 17640;

SoundSource::SoundSource(std::shared_ptr<WAVHandle> soundHandle, const glm::vec3& position, bool looping, const Listener& listener) 
	: m_soundHandle(soundHandle) 
	, m_position(position) 
	, m_looping(looping)
	, m_streamPosition(0)
	, m_listener(listener) {
	alGenSources(1, &m_id);

	for (int i = 0; i < 2; ++i) {
		loadNextChunk(m_buffers[i].getId());
	}
}

SoundSource::~SoundSource() throw() {
	stop();
	alDeleteSources(1, &m_id);
}

void SoundSource::update() {
	ALint state;
	alGetSourcei(m_id, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING)
		return;

	int processed = 0;
	alGetSourcei(m_id, AL_BUFFERS_PROCESSED, &processed);

	while (processed--) {
		ALuint buffer;

		alSourceUnqueueBuffers(m_id, 1, &buffer);
		if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to unqueue buffer");

		if (m_streamPosition >= m_soundHandle->size()) {
			if (m_looping) {
				m_streamPosition = 0;
			} else {
				stop();
				break;
			}
		}
		
		loadNextChunk(buffer);
	}
}

void SoundSource::play() {
	alSourcePlay(m_id);
}

void SoundSource::stop() {
	alSourceStop(m_id);

	int queued = 0;
	alGetSourcei(m_id, AL_BUFFERS_QUEUED, &queued);

	while (queued--) {
		ALuint buffer;
		alSourceUnqueueBuffers(m_id, 1, &buffer);
		if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to unqueue buffer");
	}
}

void SoundSource::setLooping(bool looping) {
	//       .
	//      ":"
	//    ___:____     |"\/"|
	//  ,'        `.    \  /
	//  |  O        \___/  |
	//~^~^~^~^~^~^~^~^~^~^~^~^~
	// Whale hi there, water you up to?

	alSourcei(m_id, AL_LOOPING, looping);
}

void SoundSource::loadNextChunk(ALuint buffer) {
	/** Defines a sample in a sound buffer */
	struct Sample {
		short m_left;
		short m_right;
	};
	
	std::vector<unsigned char> chunkData = std::move(m_soundHandle->getChunk(m_streamPosition, CHUNK_SIZE));
	std::vector<double> right(chunkData.size() / 4);
	std::vector<double> left(chunkData.size() / 4);

	const int MAX_SHORT = 32768;
	for (int i = 0; i < chunkData.size(); i += 4) {
		Sample* sample = (Sample*)&chunkData[i];

		right[i / 4] = ((double)sample->m_right) / MAX_SHORT;
		left[i / 4]  = ((double)sample->m_left) / MAX_SHORT;
	}

	// Apply stereo panning to the newly loaded chunk before sending it to OpenAL
	glm::vec3 displacement = m_position - m_listener.m_position;
	glm::vec3 direction = displacement;
	if (direction == glm::vec3(0,0,0))
		direction = m_listener.m_facing;
	direction = glm::normalize(direction);

	float distanceSquared = glm::dot(displacement, displacement);
	float dotRight = glm::dot(direction, m_listener.getRight());
	float dotLeft = glm::dot(direction, m_listener.getLeft());
	
	for (int i = 0; i < chunkData.size(); i += 4) {
		Sample* sample = (Sample*)&chunkData[i];
		
		float left = (float)(sample->m_left) / MAX_SHORT;
		float right = (float)(sample->m_right) / MAX_SHORT;

		PanVolume vol = constantPower(dotRight);
		float distanceFactor = 1.0f / (1.0f + 0.005 * distanceSquared);

		right *= vol.right * distanceFactor, 0.2f;
		left *= vol.left * distanceFactor, 0.2f;

		sample->m_left = (short) (left * MAX_SHORT);
		sample->m_right = (short) (right * MAX_SHORT);
	}
	

	// copy the buffer to OpenAL
	/*if (chunkData.size() > 0) {
		alBufferData(buffer, m_soundHandle->getFormat(), chunkData, bytesRead, m_soundHandle->getSampleRate());
		if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to assign buffer data");

		alSourceQueueBuffers(m_id, 1, &buffer);
		if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to queue buffer");

		m_streamPosition += bytesRead;
	}*/
}

SoundSource::PanVolume SoundSource::constantPower(float position) const {
	PanVolume result;
	const float SQRT2INV = 0.707107f;
	const float PIOVER4 = 0.785398f;
	
	float angle = position * PIOVER4;

	result.left = SQRT2INV * (cos(angle) - sin(angle));
	result.right = SQRT2INV * (cos(angle) + sin(angle));

	return result;
}