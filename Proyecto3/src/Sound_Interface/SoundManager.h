#ifndef SOUND_MANAGER_H_
#define SOUND_MANAGER_H_

#include <string>
#include <map>
using namespace std;

class nap_vector3;
class nap_transform;

namespace FMOD {
	class System;
	class Channel;
	class ChannelGroup;
	class Sound;
}

// interface for irrklang (SoundManager)
class SoundManager
{
private:
	static SoundManager* instance_;                                       //singleton pattern
	const string soundsRoute = ".\\Assets\\Sound\\";
	std::map<string, FMOD::Sound*> sounds;
	FMOD::ChannelGroup* masterGroup;

	nap_transform* listenerTransform;                                     // the transform of the listener (player)

	// FMOD
	FMOD::System* system;

	SoundManager();
	virtual ~SoundManager();

	void updateListener();

public:
	static SoundManager* getSingleton(); // you can obtain/shutdown the instance of the singleton
	static void shutdownSingleton();

	void update();

	void setListenerTransform(nap_transform* trans);       //ESPERAR A QUE DIEGO HAGA LA CONVERSION A VEC3

	void load(const string& soundName, int mode);

	FMOD::Channel* playSound(const string& soundName, int numLoops = 0, bool startPaused = false, bool track = true);

	void stopSounds();

	FMOD::System* getEngine();
	void setAllVolumes(float v);
};

#endif /* SOUND_MANAGER_H_ */

