//Nap_Time_Studios

#ifndef DIRECTIONAL__SOUND__COMP_H_
#define DIRECTIONAL__SOUND__COMP_H_

#include "SoundEmitterComponent.h"

class DirectionalSoundComponent : public SoundEmitterComponent
{
private:
	// sound directionality
	float insideConeAngle;
	float outsideConeAngle;
	float outsideVolume;
	// audition range
	float minDistance;
	float maxDistance;

	bool useOwnerDirection;
	nap_vector3 selfDirection;

	virtual void playSound();

public:
	inline DirectionalSoundComponent(nap_json const & cfg, GameObject* owner) : SoundEmitterComponent(cfg, owner) {};
	inline virtual ~DirectionalSoundComponent() {};
	virtual void setUp();

	virtual void update(GameObject * o, double time);
	virtual void receive(Message * msg);
};

#endif /* DIRECTIONAL__SOUND__COMP_H_ */

