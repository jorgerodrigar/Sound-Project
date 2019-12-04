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
}

// interface for irrklang (SoundManager)
class SoundManager
{
private:
	static SoundManager* instance_;                                       //singleton pattern
	const string soundsRoute = ".\\Assets\\Sound\\";
	int unmodifiedSounds = 0;                                             // number of sounds with no specific name
	std::map<string, pair<FMOD::Channel*, nap_vector3*>> sounds; // 3Dsounds already played, with their position
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

	// playing 3D/2D sounds... (3D will need a pointer to the emitter position
	FMOD::Channel* playSound(const string& routeName, nap_vector3* pos,
		bool playLooped = false, bool startPaused = false, string customName = "", bool track = false);

	void stopSoundByName(const string& name);

	bool isPlaying(const string& name);

	FMOD::Channel* findByName(const string& name);
	void stopSounds();

	FMOD::System* getEngine();
	void setAllVolumes(float v);
	void setVolumeByName(const string& name, float v);
};

#endif /* SOUND_MANAGER_H_ */

