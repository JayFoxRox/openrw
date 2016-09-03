#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <map>
#include <string>
#include <vector>

class SoundManager
{
public:
	SoundManager();
	~SoundManager();
	
//FIXME: This should be handled elsewhere
#if 0
	bool loadSound(const std::string& name, const std::string& fileName);
	bool isLoaded(const std::string& name);
	void playSound(const std::string& name);
	void pauseSound(const std::string& name);
	bool isPlaying(const std::string& name);
	
	bool playBackground(const std::string& fileName);

	bool loadMusic(const std::string& name, const std::string& fileName);
	void playMusic(const std::string& name);
	void stopMusic(const std::string& name);
	
	void pause(bool p);
#endif

//private:

  // Class which manages loading sounds via callback
  class SoundLoader {
  private:

    // Stream informatoin
    bool finalized; // If this is true, stream information is locked
    bool stereo; // This is the first stereo (otherwise mono) information
    unsigned int sampleRate; // This is the first encountered sample rate

    // This is a virtual function to sample data
    virtual std::vector<int16_t> read(uint64_t offset, uint64_t samples) = 0;
  };

  // Class which plays sounds (Dependend on used API)
	class Sound
	{
  private:
    //std::shared_ptr<SoundLoader>
    SoundLoader* loader;
    uint64_t offset; // Absolute offset in loader

		ALuint source;
		std::vector<ALuint> buffers;
	};

private:

	bool initializeOpenAL();
	ALCcontext* alContext = nullptr;
	ALCdevice* alDevice = nullptr;

#if 0
	std::map<std::string, Sound> sounds;
	std::map<std::string, MADStream> musics;
	std::string backgroundNoise;
#endif
};
