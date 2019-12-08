//Nap_Time_Studios
#ifndef SOUND_EMITTER_COMP_H_
#define SOUND_EMITTER_COMP_H_

#include "Component.h"

namespace FMOD {
	class Channel;
}

// it allows its owner to play a sound
class SoundEmitterComponent : public Component
{
protected:
	string routeName;

	bool threeD;
	nap_transform* emitterPos; // only if it is a 3D sound

	int numLoops;
	bool startPaused;
	bool track;

	float volume;
	FMOD::Channel* channel;

	// some methods
	void playSound();
	void stopSound();
	void setVolume(float v);

	virtual void configActive();

public:
	inline SoundEmitterComponent(nap_json const & cfg, GameObject* owner) : Component(cfg, owner) {};
	inline virtual ~SoundEmitterComponent() { };
	virtual void setUp();

	virtual void update(GameObject * o, double time);
	virtual void receive(Message * msg);
};

#endif /* SOUND_EMITTER_COMP_H_ */
