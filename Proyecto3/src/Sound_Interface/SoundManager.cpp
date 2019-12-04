#include "SoundManager.h"
#include <LogSystem.h>
#include <Transforms.h>
#include <irrKlang.h>
#include <fmod.hpp>

using namespace irrklang;
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

	// 3Dsounds: updates each playing sound position and erases the stopped sounds
	for (map<string, pair<FMOD::Channel*, nap_vector3*>>::iterator it = sounds.begin(); it != sounds.end(); it++) {
		FMOD_VECTOR pos;
		pos.x = it->second.second->x_;	pos.y = it->second.second->y_;	pos.z = it->second.second->z_;

		FMOD_VECTOR vel;
		vel.x = vel.y = vel.z = 0;

		it->second.first->set3DAttributes(&pos, &vel);
	}

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

// it plays a 3D sound. If the track bool is true, it also returns the sound.
// If you want to change a sound in execution, you must name it to have a later reference
Channel* SoundManager::playSound(const string& routeName, nap_vector3* pos, bool playLooped, bool startPaused, string customName, bool track)
{
	Channel* channel;
	//system->playSound(Resources::sounds(routeName), 0, startPaused, &channel);

	channel->setLoopCount(playLooped);
	//irrklang::ISound* sound3D = engine->play3D((soundsRoute + routeName).c_str(), irrklang::vec3df(pos->x_, pos->y_, pos->z_), playLooped, startPaused, true);

	if (customName != "") {
		sounds.erase(customName);
		sounds.insert({ customName, {channel, pos} }); // you will be able to have a reference to that sound later
	}
	else {
		sounds.insert({ "__" + to_string(unmodifiedSounds), {channel, pos} });
		unmodifiedSounds++;
	}

	if (track) return channel;
	else return nullptr;
}

void SoundManager::stopSoundByName(const string & name)
{
	Channel* channel = findByName(name);
	channel->stop();
	sounds.erase(name);
}

bool SoundManager::isPlaying(const string& name) {
	Channel* channel = findByName(name);
	bool playing = false;
	channel->isPlaying(&playing);
	return playing;
}

// it returns the 3D sound with the name specified
Channel* SoundManager::findByName(const string& name) {
	map<string, pair<FMOD::Channel*, nap_vector3*>>::iterator it = sounds.find(name);
	if (it != sounds.end())
		return it->second.first;
	else return nullptr;
}

void SoundManager::stopSounds()
{
	masterGroup->stop();
	sounds.clear();
	unmodifiedSounds = 0;
}

FMOD::System * SoundManager::getEngine()
{
	return system;
}

void SoundManager::setAllVolumes(float v) {
	masterGroup->setVolume(v);
}

void SoundManager::setVolumeByName(const string& name, float v)
{
	auto it = sounds.find(name);
	if (it != sounds.end())it->second.first->setVolume(v);
}

SoundManager::SoundManager(): system(nullptr)
{
	System_Create(&system); // Creamos el objeto system

	// 128 canales (numero maximo que podremos utilizar simultaneamente)
	system->init(128, FMOD_INIT_NORMAL, 0); // Inicializacion de FMOD
	system->set3DSettings(1.0f, 1.0f, 1.0f); // doppler/factor de escalado de distancia/rolloff

	system->getMasterChannelGroup(&masterGroup);
}

SoundManager::~SoundManager()
{
	system->release();
}
