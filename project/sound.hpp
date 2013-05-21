#ifndef SOUND_HPP
#define SOUND_HPP

#include <memory>
#include <string>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <glm/glm.hpp>

/** Forward declarations */
class WAVHandle;
class SoundSource;
class SoundBuffer;


/** Reads and stores WAV file data */
class WAVHandle {
public:
	WAVHandle(const std::string& filepath);

	ALenum getFormat() const;
	unsigned int getChannelCount() const { return m_channelCount; }
	unsigned int getSampleRate() const { return m_sampleRate; }
	unsigned int getBytesPerSample() const { return m_bytesPerSample; }
	size_t size() const { return m_data.size(); }

	int getChunk(unsigned int streamPosition, unsigned int chunkSize, unsigned char*& data); 
private:
	unsigned int m_channelCount;
	unsigned int m_sampleRate;
	unsigned int m_bytesPerSample;	// Number of bytes per sample (including all channels)
	std::vector<unsigned char> m_data;
};

/** Abstracts the OpenAL concept of a buffer */
class SoundBuffer {
public:
	SoundBuffer();
	~SoundBuffer() throw();

	ALuint getId() const { return m_id; }
private:
	ALuint m_id;
};

/** Abstracts the OpenAL concept of a source. It also implements double buffering. */
class SoundSource {
public:
	SoundSource(std::shared_ptr<WAVHandle> soundHandle, const glm::vec3& position, bool looping);
	~SoundSource() throw();

	void update();
	void play();
	void stop();
	void setLooping(bool looping);
private:
	ALuint m_id;
	glm::vec3 m_position;
	std::shared_ptr<WAVHandle> m_soundHandle;
	SoundBuffer m_buffers[2];
	bool m_looping;
	unsigned int m_streamPosition;

	void loadNextChunk(ALuint buffer);

	static const int CHUNK_SIZE;
};



#endif