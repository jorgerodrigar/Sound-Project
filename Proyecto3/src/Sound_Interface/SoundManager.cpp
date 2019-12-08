#include "SoundManager.h"
#include <LogSystem.h>
#include <Transforms.h>
#include <fmod.hpp>

using namespace FMOD;

SoundManager* SoundManager::instance_ = nullptr;

SoundManager* SoundManager::getSingleton() {
	if (instance_ == nullptr) {
		instance_ = new SoundManager();
	}

	return instance_;
}

void SoundManager::shutdownSingleton() {
	if (instance_ == nullptr) return;

	delete instance_;
	instance_ = nullptr;
}

// update of all the positions and the engine
void SoundManager::update() {
	// listener transform (position, orientation)
	updateListener();

	system->update();
}

void SoundManager::updateListener()
{
	if (listenerTransform == nullptr)
		LogSystem::Log("listenerTransform hasn´t been initialized. Call singleton->getEngine->setListener at least once", LogSystem::SOUND);
	else {
		nap_vector3 dir = listenerTransform->q_.toNapVec3(nap_vector3(0, 0, 1));

		FMOD_VECTOR pos;
		pos.x = listenerTransform->p_.x_; pos.y = listenerTransform->p_.y_;	pos.z = listenerTransform->p_.z_;	

		FMOD_VECTOR vel;
		vel.x = vel.y = vel.z = 0;

		FMOD_VECTOR up;
		up.x = 0;	up.y = 1;	up.z = 0;

		FMOD_VECTOR at;
		at.x = dir.x_;	at.y = dir.y_;	at.z = dir.z_;

		system->set3DListenerAttributes(0, &pos, &vel, &up, &at); // PARSING NEEDED
	}
}

//Sets the position of the listener. Call this method at least once after you call getsingleton for the first time. Doesn´t need to be updated since its a pointer
void SoundManager::setListenerTransform(nap_transform* trans)
{
	listenerTransform = trans;
	updateListener();
}

void SoundManager::load(const string& soundName, int mode)
{
	if (sounds.find(soundName) == sounds.end()) {
		FMOD::Sound* sound;
		system->createSound((soundsRoute + soundName).c_str(), mode | FMOD_LOOP_NORMAL, 0, &sound);
		sound->setMode(FMOD_INIT_3D_RIGHTHANDED);
		sounds.insert({ soundName, sound });
	}
}

// it plays a 3D sound. If the track bool is true, it also returns the sound.
// If you want to change a sound in execution, you must name it to have a later reference
Channel* SoundManager::playSound(const string& soundName, int numLoops, bool startPaused, bool track)
{
	try {
		Channel* channel;
		system->playSound(sounds.at(soundName), 0, startPaused, &channel);
		channel->setLoopCount(numLoops);
		if (track) return channel;
		else return nullptr;
	}
	catch (exception e) {
		string skk = soundName;
		std::map<string, FMOD::Sound*> soundskk = sounds;
	}
	return nullptr;

}

void SoundManager::stopSounds()
{
	masterGroup->stop();
}

FMOD::System * SoundManager::getEngine()
{
	return system;
}

void SoundManager::setAllVolumes(float v) {
	masterGroup->setVolume(v);
}

SoundManager::SoundManager(): system(nullptr)
{
	System_Create(&system); // Creamos el objeto system

	// 128 canales (numero maximo que podremos utilizar simultaneamente)
	system->init(128, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0); // Inicializacion de FMOD
	system->set3DSettings(1.0f, 1.0f, 1.0f); // doppler/factor de escalado de distancia/rolloff

	system->getMasterChannelGroup(&masterGroup);
}

SoundManager::~SoundManager()
{
	system->release();
}
