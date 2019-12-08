#include "SoundEmitterComponent.h"

#include <SoundManager.h>
#include <fmod.hpp>

#include "Messages.h"

using namespace FMOD;

void SoundEmitterComponent::setUp() {
	if (isInited()) return;
	setInited();

	if (FIND(cfg_, "routeName")) { string auxRouteName = cfg_["routeName"]; routeName = auxRouteName; }
	else routeName = "ophelia.mp3";

	if (FIND(cfg_, "3D")) threeD = cfg_["3D"];
	else threeD = false;

	emitterPos = owner_->getTransPtr();

	if (FIND(cfg_, "loop")) numLoops = cfg_["loop"];
	else numLoops = 0;

	if (FIND(cfg_, "startPaused")) startPaused = cfg_["startPaused"];
	else startPaused = false;

	if (FIND(cfg_, "volume")) volume = cfg_["volume"];
	else volume = 1;

	int mode = FMOD_2D;
	if (threeD)mode = FMOD_3D;

	SoundManager::getSingleton()->load(routeName, mode);
}

void SoundEmitterComponent::configActive() {
	if (active_) {
		if (!startPaused)
			playSound();
	}
	else stopSound();
}

void SoundEmitterComponent::update(GameObject * o, double time) {
	bool playing;
	channel->isPlaying(&playing);
	if (channel != nullptr && threeD && playing) {
		FMOD_VECTOR pos;
		pos.x = emitterPos->p_.x_; pos.y = emitterPos->p_.y_;	pos.z = emitterPos->p_.z_;
		FMOD_VECTOR vel;
		vel.x = vel.y = vel.z = 0;
		channel->set3DAttributes(&pos, &vel);
	}
}

void SoundEmitterComponent::receive(Message * msg) {
	if (msg->id_ == STATE_IN) {
		configActive();
	}
	else if (msg->id_ == PLAY_SOUND) {
		playSound();
	}
	else if (msg->id_ == STOP_SOUND) {
		stopSound();
	}
}

void SoundEmitterComponent::playSound() {
	channel = SoundManager::getSingleton()->playSound(routeName, numLoops, false, true);

	setVolume(volume);
}

void SoundEmitterComponent::stopSound() {
	if (channel != nullptr) channel->stop();
}

void SoundEmitterComponent::setVolume(float v) {
	volume = v;
	if (channel != nullptr) channel->setVolume(volume);
}

#include "GOFactory.h"
REGISTER_TYPE(SoundEmitterComponent);