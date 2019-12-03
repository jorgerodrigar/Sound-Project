#include "SoundFMOD.h"

SoundFMOD::SoundFMOD(System* system):system_(system), buffer_(nullptr), channel_(nullptr), isPlaying_(false)
{
}

SoundFMOD::~SoundFMOD()
{
	channel_->stop();
}

void SoundFMOD::load(const char* name, int MODE)
{
	system_->createSound(name, MODE,	0, &buffer_);
}

void SoundFMOD::play()
{
	isPlaying_ = true;
	system_->playSound(buffer_, 0, false, &channel_); // 0 es el grupo de canales (global)
}

void SoundFMOD::stop()
{
	isPlaying_ = false;
	channel_->stop();
}

void SoundFMOD::pause(bool paused)
{
	isPlaying_ = false;
	channel_->setPaused(paused);
}

bool SoundFMOD::isPlaying()
{
	return isPlaying_;
}

void SoundFMOD::fadePoint(unsigned long long samples, float volume)
{
	unsigned long long currentTime = 0;
	float iniVolume = 0.0f;
	channel_->getVolume(&iniVolume);
	channel_->getDSPClock(nullptr, &currentTime);
	channel_->addFadePoint(currentTime, iniVolume);
	channel_->addFadePoint(currentTime + samples, volume);
}

void SoundFMOD::setVolume(float volume)
{
	channel_->setVolume(volume);
}

void SoundFMOD::setPan(float pan)
{
	channel_->setPan(pan);
}

void SoundFMOD::setPitch(float pitch)
{
	channel_->setPitch(pitch);
}
