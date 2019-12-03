#include "checkML.h" //for memory leaks
#include "Game.h"
#include <GlobalConfig.h>
#include <GoFactory.h>
#include <LogSystem.h>

//fowarded
#include <JsonReader.h>
#include <GameStateMachine.h>
#include <MessageSystem.h>
#include <RenderSystemManager.h>
#include <RenderSystemInterface.h>
#include <PhysicsSystemManager.h>
#include <SoundManager.h>

#include <SDL_events.h>	//events
#include <TimeSystem.h>	//timing

Game::Game() {
	initGame();
}

Game::~Game() {
	//closed on loop end
}

//creates the gameStateMachine, pushes the first state...
void Game::initGame() {
	//Logging system
	LogSystem::resetLogFile();
	LogSystem::Log("initializing...", LogSystem::GAME, {__FILE__, __LINE__});

	//Get/create the singleton instances
	physicsManager_ = PhysicsSystemManager::getSingleton();
	renderManager_ = RenderSystemManager::getSingleton();
	soundManager_ = SoundManager::getSingleton();
	messageSystem_ = MessageSystem::getSingleton();

	//Initialize level
	SceneReader::getSingleton(); //load prefabs
	LogSystem::cls();
	LogSystem::Log("singletons done -> initializing level...", LogSystem::GAME);

	gsm_ = GameStateMachine::getSingleton();
	auto menu = gsm_->loadLevel(GlobalCFG::strings["init_scene_name"]); //gsm uses the parser + factory
	gsm_->pushState(menu); //you can push it already and add more things later

	//all done
	LogSystem::cls();
	LogSystem::Log("initialized", LogSystem::GAME);
}

//shutdown singletons etc in reverse order
void Game::closeGame() {
	LogSystem::Log("closing...", LogSystem::GAME);
	LogSystem::cls();

	gsm_->shutdownSingleton();

	//Close singleton instances
	SceneReader::shutdownSingleton();
	MessageSystem::shutdownSingleton();
	SoundManager::shutdownSingleton();
	RenderSystemInterface::shutdownSingleton();
	RenderSystemManager::shutdownSingleton();
	PhysicsSystemManager::shutdownSingleton();
	GOFactory::cleanFactories();

	LogSystem::Log("closed", LogSystem::GAME);
}

///////////////////////////////////////////////////////////////////

void Game::start() {
	LogSystem::Log("starting game", LogSystem::GAME);
	run();
}

void Game::stop() {
	LogSystem::Log("stopping game", LogSystem::GAME);
	exit_ = true;
}

///////////////////////////////////////////////////////////////////

//main game loop, ends with exit_
void Game::run() {
	exit_ = false;

#ifndef FIXED_STEP
	TimeSystem::StartCounter();
#endif

	while (!exit_ && !gsm_->stackIsEmpty()) {
#ifdef FIXED_STEP
		if (t < (1.0f / 30.0f)) {
			//fprintf(stderr, "Time: %f\n", stepTime);
			stepTime += t;
		}
		else stepTime = 1.0f / 30.0f;

		if (stepTime >= (1.0f / 30.0f)) {
			t = stepTime;
			stepTime = 0.0f;
		}
#else
		double t = TimeSystem::GetCounter();
#endif
		//LogSystem::Log("time step: ", t, LogSystem::GAME);

		// STEP PHYSICS
		//LogSystem::Log("main physics", LogSystem::GAME);
		physicsManager_->stepPhysics(t);
		//retrieve collisions (add to events queue? or messages?)
		physicsManager_->updateNodes();

		// EVENTS
		//LogSystem::Log("main handleEvents", LogSystem::GAME);
		handleEvents(); //atm sets exit
		//handleCollisions(); //if no events queue?

		// LOGIC
		//LogSystem::Log("main update", LogSystem::GAME);
		gsm_->update(t); //and its sub-parts like lateUpdate

		// RENDER OGRE
		//LogSystem::Log("main render", LogSystem::GAME);
		renderManager_->renderFrame(t);

		// SOUND
		//LogSystem::Log("main sound", LogSystem::GAME);
		soundManager_->update();
	}

	closeGame();
}

//handle main events (Ex. quit) and call the gameStateMachine machine update
void Game::handleEvents() {
	SDL_Event evt;
	while (SDL_PollEvent(&evt)) {
		bool handled = false; //stop global propagation

		//window x botton - close game
		if (evt.type == SDL_QUIT) {
			handled = true;
			stop();
		}
#if _DEBUG
		else if (evt.type == SDL_KEYDOWN) {
			switch (evt.key.keysym.sym) {
				case SDLK_ESCAPE:
					handled = true;
					stop();
					break;
			}
		}
#endif

		if (!handled) handled = renderManager_->handleEvents(evt);
		if (!handled) handled = gsm_->handleEvents(evt);
	}
}