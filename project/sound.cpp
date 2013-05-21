#include "sound.hpp"
#include <iostream>
#include <fstream>
#include <r2tk\r2-exception.hpp>

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

int WAVHandle::getChunk(unsigned int streamPosition, unsigned int chunkSize, unsigned char*& data) {
	data = &m_data[streamPosition];

	int remaining = m_data.size() - streamPosition;
	remaining = (remaining >= 0) ? remaining : 0;
	return (remaining < chunkSize) ? remaining : chunkSize;
}


SoundBuffer::SoundBuffer() {
	alGenBuffers(1, &m_id);
}

SoundBuffer::~SoundBuffer() throw() {
	alDeleteBuffers(1, &m_id);
}


const int SoundSource::CHUNK_SIZE = 30872;

SoundSource::SoundSource(std::shared_ptr<WAVHandle> soundHandle, const glm::vec3& position, bool looping) 
	: m_soundHandle(soundHandle) 
	, m_position(position) 
	, m_looping(looping)
	, m_streamPosition(0) {
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
}

void SoundSource::loadNextChunk(ALuint buffer) {
	std::cout << "Loading new chunk for buffer " << buffer << std::endl;

	unsigned char* chunkData;
	int bytesRead = m_soundHandle->getChunk(m_streamPosition, CHUNK_SIZE, chunkData);

	if (bytesRead > 0) {
		alBufferData(buffer, m_soundHandle->getFormat(), chunkData, bytesRead, m_soundHandle->getSampleRate());
		if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to assign buffer data");

		alSourceQueueBuffers(m_id, 1, &buffer);
		if (alGetError() != AL_NO_ERROR)
			throw r2ExceptionRuntimeM("Failed to queue buffer");

		m_streamPosition += bytesRead;
	}
}