#pragma once

#ifndef _MSC_VER
#include <sndfile.h>
#endif
#include <AL/al.h>
#include <AL/alc.h>

#include <map>
#include <string>
#include <vector>

class MADStream;

class SoundManager
{
public:
	SoundManager();
	~SoundManager();
	
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
	
private:

	class SoundSource
	{
		friend class SoundManager;
		friend class SoundBuffer;
	public:
		void loadFromFile(const std::string& filename);
	private:
#ifndef _MSC_VER
		SF_INFO fileInfo;
		SNDFILE* file;
#endif
		std::vector<uint16_t> data;
	};

	class SoundBuffer
	{
		friend class SoundManager;
	public:
		SoundBuffer();
		bool bufferData(SoundSource& soundSource);
	private:
		ALuint source;
		ALuint buffer;
	};

	struct Sound
	{
		SoundSource source;
		SoundBuffer buffer;
		bool isLoaded = false;
	};

	bool initializeOpenAL();

	ALCcontext* alContext = nullptr;
	ALCdevice* alDevice = nullptr;

	std::map<std::string, Sound> sounds;
#ifndef _MSC_VER
	std::map<std::string, MADStream> musics;
#endif
	std::string backgroundNoise;
};
