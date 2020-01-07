//Nap_Time_Studios

#ifndef REVERB__COMP_H_
#define REVERB__COMP_H_

#include "Component.h"
#include <string>

namespace FMOD {
	class Reverb3D;
}

class FMOD_REVERB_PROPERTIES;

class ReverbComponent : public Component
{
private:
	FMOD::Reverb3D* reverb;
	nap_vector3 position;
	float minDist;
	float maxDist;

	int activateId, deactivateId;

	FMOD_REVERB_PROPERTIES* reverbProperties;

public:
	inline ReverbComponent(nap_json const & cfg, GameObject* owner) : Component(cfg, owner) {};
	inline virtual ~ReverbComponent();
	virtual void setUp();
	virtual void lateSetUp();
	virtual void receive(Message* msg);
};

#endif /* REVERB__COMP_H_ */

