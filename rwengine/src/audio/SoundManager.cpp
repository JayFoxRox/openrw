#include <audio/SoundManager.hpp>

#include "audio/alCheck.hpp"

#include <array>
#include <iostream>

SoundManager::SoundManager()
{
  alDevice = alcOpenDevice(NULL);
	if ( ! alDevice) {
		std::cerr << "Could not find OpenAL device!" << std::endl;
		return false;
	}

	alContext = alcCreateContext(alDevice, NULL);
	if ( ! alContext) {
		std::cerr << "Could not create OpenAL context!" << std::endl;
		return false;
	}

	if ( ! alcMakeContextCurrent(alContext)) {
		std::cerr << "Unable to make OpenAL context current!" << std::endl;
		return false;
	}

	return true;
}

SoundManager::~SoundManager()
{
	// De-initialize OpenAL
	if(alContext) {
		alcMakeContextCurrent(NULL);
		alcDestroyContext(alContext);
	}

	if(alDevice) {
		alcCloseDevice(alDevice);
  }
}

#if 0
void SoundManager::SoundSource::loadFromFile(const std::string& filename)
{
	fileInfo.format = 0;
	file = sf_open(filename.c_str(), SFM_READ, &fileInfo);

	if (file) {
		size_t numRead = 0;
		std::array<int16_t, 4096> readBuffer;

		while ((numRead = sf_read_short(file, readBuffer.data(), readBuffer.size())) != 0) {
			data.insert(data.end(), readBuffer.begin(), readBuffer.begin() + numRead);
		}
	} else {
		std::cerr << "Error opening sound file \"" << filename << "\": " << sf_strerror(file) << std::endl;
	}
}

SoundManager::SoundBuffer::SoundBuffer()
{
	alCheck(alGenSources(1, &source));
	alCheck(alGenBuffers(1, &buffer));

	alCheck(alSourcef(source, AL_PITCH, 1));
	alCheck(alSourcef(source, AL_GAIN, 1));
	alCheck(alSource3f(source, AL_POSITION, 0, 0, 0));
	alCheck(alSource3f(source, AL_VELOCITY, 0, 0, 0));
	alCheck(alSourcei(source, AL_LOOPING, AL_FALSE));
}


bool SoundManager::SoundBuffer::bufferData(std::vector<int16_t> data, bool stereo)
{
	alCheck(alBufferData(
		buffer,
		stereo ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		&soundSource.data.front(),
		soundSource.data.size() * sizeof(uint16_t),
		soundSource.fileInfo.samplerate
	));
	alCheck(alSourcei(source, AL_BUFFER, buffer));

	return true;
}

bool SoundManager::loadSound(const std::string& name, const std::string& fileName)
{
	Sound* sound = nullptr;
	auto sound_iter = sounds.find(name);

	if (sound_iter != sounds.end()) {
		sound = &sound_iter->second;
	} else {
		auto emplaced = sounds.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple());
		sound = &emplaced.first->second;

		sound->source.loadFromFile(fileName);
		sound->isLoaded = sound->buffer.bufferData(sound->source);
	}

	return sound->isLoaded;
}
bool SoundManager::isLoaded(const std::string& name)
{
	if (sounds.find(name) != sounds.end()) {
		return sounds[name].isLoaded;
	}

	return false;
}

#endif

// Retrieves data from the file loader and appends it to the buffer queue
void Sound::queueBuffers(std::vector<ALuint> buffers) {
  for(auto& buffer : buffers) {
    //FIXME: Repeat this if the current chunk was smaller than the expected chunksize
    auto data = loader->read(offset, CHUNK_SIZE);
    alCheck(alBufferData(
      buffer,
      loader->isStereo() ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
      data.begin(),
      data.size() * sizeof(uint16_t),
      loader->getSamplerate()
    ));
    position += data.size / loader->isStereo() ? 2 : 1;
    alSourceQueueBuffers(source, 1, &buffer);
    if(alGetError() != AL_NO_ERROR) {
      fprintf(stderr, "Error buffering :(\n");
      return 1;
    }
  }
}

Sound* SoundManager::createSound(SoundLoader* loader, bool stream = true)
{
  Sound* sound = new Sound();
  sound->loader = loader;

  // Check if this file should *really* be streamed or if it's small enough
  if (loader->getSize() <= CHUNK_SIZE) {
    stream = false;
  }

  // Create a source to play from buffers
	alCheck(alGenSources(1, &source));

  // Default configuration
	alCheck(alSourcef(source, AL_GAIN, 1.0f));
	alCheck(alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f));
	alCheck(alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f));
  setPitch(1.0f);
  setLooping(false);

  // Create buffers to play from and fill them with data
  //FIXME: If this is not streaming we can just use the same buffer shared accross all sources
  unsigned int chunkCount = stream ? 3 : 1;
  buffers.reserve(chunkCount);
  alCheck(glGenBuffers(chunkCount, buffers.begin()));
  Sound::queueBuffers(buffers);

  // List this to the sources which will be managed
  sources.add(source);
}

void SoundManager::update() {
  for(auto source : sources) {
    // Check if any source has finished processing a buffer
    ALint freeBufferCount;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &freeBufferCount);
    if(freeBufferCount <= 0) {
      continue;
    }

    // Remove the processed buffers, they are free again
    std::vector<ALuint> buffers;
    buffers.reserve(freeBufferCount);
    alSourceUnqueueBuffers(source, freeBufferCount, buffers);

    // As those buffers are free, we'll requeue them
    queueBuffers(source, buffers);
  }
}

// Returns the current time in milliseconds
uint64_t Sound::getTime() {
  // Get the actively playing buffer and where that buffer is in global time
  uint64_t globalOffset = 
  // Add the local offset within that buffer
  uint64_t localOffset = 
  return globalOffset + localOffset;
}
void Sound::setPitch(float pitch) {
	alCheck(alSourcef(source, AL_PITCH, pitch));
}
void Sound::setLooping(bool looping) {
	alCheck(alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE));
}
void Sound::play() {
	alCheck(alSourcePlay(source));
}
void Sound::pause() {
	alCheck(alSourcePause(source));
}
bool Sound::isPlaying() {
  ALint sourceState;
  alCheck(alGetSourcei(source, AL_SOURCE_STATE, &sourceState));
  return AL_PLAYING == sourceState;
}

#if 0
bool SoundManager::playBackground(const std::string& fileName)
{
	if (this->loadSound(fileName, fileName)) {
		backgroundNoise = fileName;
		this->playSound(fileName);
		return true;
	}

	return false;
}

bool SoundManager::loadMusic(const std::string& name, const std::string& fileName)
{
	MADStream* music = nullptr;
	auto music_iter = musics.find(name);

	if (music_iter != musics.end()) {
		music = &music_iter->second;
	} else {
		auto emplaced = musics.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple());
		music = &emplaced.first->second;
	}

	return music->openFromFile(fileName);
}
void SoundManager::playMusic(const std::string& name)
{
	auto music = musics.find(name);
	if (music != musics.end()) {
		music->second.play();
	}
}
void SoundManager::stopMusic(const std::string& name)
{
	auto music = musics.find(name);
	if (music != musics.end()) {
		music->second.stop();
	}
}

void SoundManager::pause(bool p)
{
	if (backgroundNoise.length() > 0) {
		if (p) {
			pauseSound(backgroundNoise);
		} else {
			playSound(backgroundNoise);
		}
	}
}
#endif
