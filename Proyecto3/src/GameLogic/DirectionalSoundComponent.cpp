#include "DirectionalSoundComponent.h"
#include <fmod.hpp>
#include "Messages.h"

void DirectionalSoundComponent::setUp()
{
	SoundEmitterComponent::setUp();

	if (FIND(cfg_, "insideConeAngle")) insideConeAngle = cfg_["insideConeAngle"];
	else insideConeAngle = 10.0f;
	if (FIND(cfg_, "outsideConeAngle")) outsideConeAngle = cfg_["outsideConeAngle"];
	else outsideConeAngle = 60.0f;
	if (FIND(cfg_, "outsideVolume")) outsideVolume = cfg_["outsideVolume"];
	else outsideVolume = 0.0f;
	if (FIND(cfg_, "minDistance")) minDistance = cfg_["minDistance"];
	else minDistance = 1.0f;
	if (FIND(cfg_, "maxDistance")) maxDistance = cfg_["maxDistance"];
	else maxDistance = 1000.0f;
	if (FIND(cfg_, "useOwnerDirection")) useOwnerDirection = cfg_["useOwnerDirection"];
	else useOwnerDirection = true;
	if (FIND(cfg_, "selfDirection")) selfDirection = nap_vector3(cfg_["selfDirection"]);
	else selfDirection = nap_vector3(0, 0, 1);

	threeD = true;
}

void DirectionalSoundComponent::playSound()
{
	if (active_) {
		bool playing = false;
		if(channel != nullptr) channel->isPlaying(&playing);
		if (!playing) {
			SoundEmitterComponent::playSound();

			channel->set3DConeSettings(insideConeAngle, outsideConeAngle, outsideVolume);
			channel->set3DMinMaxDistance(minDistance, maxDistance);
		}
	}
}

void DirectionalSoundComponent::update(GameObject * o, double time)
{
	if (active_ && channel != nullptr) {
		bool playing;
		channel->isPlaying(&playing);
		if (threeD && playing) {
			FMOD_VECTOR vel;
			vel.x = vel.y = vel.z = 0;
			channel->set3DAttributes(&emitterTrans->p_.fmod(), &vel);

			nap_vector3 dir;
			if (useOwnerDirection)
				dir = emitterTrans->q_.toNapVec3(nap_vector3(0, 0, 1));
			else
				dir = selfDirection;

			channel->set3DConeOrientation(&dir.fmod());
		}
	}
}

void DirectionalSoundComponent::receive(Message * msg)
{
}

#include "GOFactory.h"
REGISTER_TYPE(DirectionalSoundComponent);

