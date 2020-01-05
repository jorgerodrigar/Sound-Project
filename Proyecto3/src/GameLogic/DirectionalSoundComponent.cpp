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
		SoundEmitterComponent::playSound();

		channel->set3DConeSettings(insideConeAngle, outsideConeAngle, outsideVolume);
		channel->set3DMinMaxDistance(minDistance, maxDistance);
	}
}

void DirectionalSoundComponent::update(GameObject * o, double time)
{
	if (active_) {
		bool playing;
		channel->isPlaying(&playing);
		if (channel != nullptr && threeD && playing) {
			FMOD_VECTOR pos;
			pos.x = emitterTrans->p_.x_; pos.y = emitterTrans->p_.y_;	pos.z = emitterTrans->p_.z_;
			FMOD_VECTOR vel;
			vel.x = vel.y = vel.z = 0;
			channel->set3DAttributes(&pos, &vel);
			FMOD_VECTOR dir;
			if (useOwnerDirection) {
				nap_vector3 aux = emitterTrans->q_.toNapVec3(nap_vector3(0, 0, 1));
				dir.x = aux.x_; dir.y = aux.y_;	dir.z = aux.z_;
			}
			else {
				dir.x = selfDirection.x_; dir.y = selfDirection.y_;	dir.z = selfDirection.z_;
			}
			channel->set3DConeOrientation(&dir);
		}
	}
}

#include "GOFactory.h"
REGISTER_TYPE(DirectionalSoundComponent);

