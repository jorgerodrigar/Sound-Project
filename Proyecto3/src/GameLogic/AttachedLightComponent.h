#ifndef ATTACHEDLIGHTCOMPONENT_H_
#define ATTACHEDLIGHTCOMPONENT_H_

#include "Component.h"

namespace Ogre {
	class SceneNode;
}

class AttachedLightComponent: public Component
{
private:
	Ogre::SceneNode* lightNode = nullptr;
	void setLight();

public:
	virtual ~AttachedLightComponent() {};
	AttachedLightComponent(nap_json const & cfg, GameObject* owner) : Component(cfg, owner) {};

	virtual void setUp();
	virtual void lateSetUp();
};

#endif //ATTACHEDLIGHTCOMPONENT_H_