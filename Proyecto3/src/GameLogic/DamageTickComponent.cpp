#include "DamageTickComponent.h"
#include "MessageSystem.h"
#include "GameStateMachine.h"


void DamageTickComponent::setUp() {
	if (isInited()) return;
	setInited();

	timer.setDuration(cfg_["timer"]);
	timer.stop();
	dmg = cfg_["dmg"];
	enterId = cfg_["enterId"];
	exitId = cfg_["exitId"];
	if(FIND(cfg_, "target")) target_ = GameStateMachine::getSingleton()->currentState()->getGameObject(cfg_["target"]);
}

void DamageTickComponent::receive(Message * msg)
{
	if (msg->id_ == (MessageId)enterId) {
		active_ = true;
		if(target_ != nullptr) MessageSystem::getSingleton()->sendMessageGameObject(&Msg_HP_DAMAGE(dmg), target_);
		MessageSystem::getSingleton()->sendMessageGameObject(&Message(PLAY_SOUND), owner_);
		timer.start();
	}
	else if (msg->id_ == (MessageId)exitId) {
		active_ = false;
		if (target_ != nullptr) MessageSystem::getSingleton()->sendMessageGameObject(&Message(END_INV), target_);
		MessageSystem::getSingleton()->sendMessageGameObject(&Message(STOP_SOUND), owner_);
		timer.stop();
	}
}

void DamageTickComponent::update(GameObject * ent, double time)
{
	if (timer.update(time)) {
		if (target_ != nullptr) MessageSystem::getSingleton()->sendMessageGameObject(&Msg_HP_DAMAGE(dmg), target_);
		timer.start();
	}
}

#include "GOFactory.h"
REGISTER_TYPE(DamageTickComponent);
