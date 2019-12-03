#ifndef SOUND_FMOD_H_
#define SOUND_FMOD_H_

#include <fmod.hpp>

using namespace FMOD;

class SoundFMOD
{
private:
	Sound* buffer_;
	System* system_;
	Channel* channel_;
	bool isPlaying_;

public:
	SoundFMOD(System* system);
	~SoundFMOD();

	void load(const char* name, int MODE = FMOD_DEFAULT);
	void play();
	void stop();
	void pause(bool paused);

	bool isPlaying();
	void fadePoint(unsigned long long samples, float volume);

	void setVolume(float volume);
	void setPan(float pan);
	void setPitch(float pitch);
};

#endif //SOUND_FMOD_H_

