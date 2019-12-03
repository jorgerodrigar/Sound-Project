//Nap_Time_Studios
#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_

#include "Component.h"
#include "UpgradeManager.h"
#include <Timer.h>

class OverlayComponent;
class GameObject;
class HPComponent;
class napTimer;

namespace Ogre {
	class TextAreaOverlayElement;
	class OverlayElement;
}

// the gameManager object would have this component
// this component manages the logic of the game and controls the HUD
// with the other OverlayComponents of its object
class GameManager : public Component
{
private:
	OverlayComponent* overlayComp;
	Ogre::OverlayElement* HPStripe;
	Ogre::OverlayElement* HPDif;
	Ogre::OverlayElement* HPContainer;
	Ogre::TextAreaOverlayElement* ScoreText;
	Ogre::TextAreaOverlayElement* RoundText;
	Ogre::TextAreaOverlayElement* MiniRoundText;

	// HP bar
	float HPwidth, HPleft, HPtop;
	float HPCwidth, HPCleft, HPCtop, HPBCdiff;
	float HPDifDecr;

	// timers
	napTimer hitTimer;
	napTimer roundTimer;

	// timer durations
	float hitTime;
	float deathTime;
	float roundTime;

	GameObject* player_;
	HPComponent* playerHP_;
	float prevHP;

	int score_;
	int enemies_; // enemies to kill in the actual round
	int round_;
	float probability_; // initial prob of a destr spawner to be activated

	std::string death_state;
	std::string hitMarker_soundEmitter;
	std::string deathMarker_soundEmitter;

	void updateUI();
	void nextRound();

	void activateDestrSpawners();

	std::vector<GameObject*>destructibleSpawners;
	UpgradeManager upgradeManager;
	bool endRound = false;

public:
	inline GameManager(nap_json const & cfg, GameObject* owner) : 
		Component(cfg, owner), score_(0), enemies_(0), round_(0),upgradeManager() {};
	inline virtual ~GameManager();
	virtual void setUp();
	virtual void lateSetUp();

	virtual void update(GameObject* o, double time);
	virtual bool handleEvents(GameObject* o, const SDL_Event& evt);
	virtual void receive(Message* msg);

	void resetPlayer();
	inline void addScore(int moreScore) { score_ += moreScore; }
};

#endif /* GAME_MANAGER_H_ */
