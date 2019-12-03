#include "UpgradeComponent.h"
#include "LogSystem.h"
#include "MessageSystem.h"
#include "GameStateMachine.h"

void UpgradeComponent::onCollision(ID * other) {
	if (!owner_->isActive()) return;
	if (other->name_ == "player") {
		owner_->setActive(false, true);
		MessageSystem::getSingleton()->sendMessageGameObject(
			&Msg_UPGRADE_TAKEN(upgrade_), GameStateMachine::getSingleton()->currentState()->getGM());
		MessageSystem::getSingleton()->sendMessageComponentName(&Message(PLAY_SOUND), "player", "UpgradeSound");

	}
}

void UpgradeComponent::setUpgrade(int upgrade)
{
	upgrade_ = upgrade;
}


#include "GOFactory.h"
REGISTER_TYPE(UpgradeComponent);
