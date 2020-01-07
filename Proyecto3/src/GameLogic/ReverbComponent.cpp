#include "ReverbComponent.h"
#include <fmod.hpp>
#include <SoundManager.h>
#include "Messages.h"

using namespace FMOD;

inline ReverbComponent::~ReverbComponent()
{
	delete reverbProperties; reverbProperties = nullptr;
	reverb->release();
}

void ReverbComponent::setUp()
{
	if (FIND(cfg_, "position")) position = nap_vector3(cfg_["position"]);
	else position = nap_vector3(0, 0, 0);
	if (FIND(cfg_, "minDist")) minDist = cfg_["minDist"];
	else minDist = 1000.0f;
	if (FIND(cfg_, "maxDist")) maxDist = cfg_["maxDist"];
	else maxDist = 1000.0f;

	activateId = cfg_["activateId"];
	deactivateId = cfg_["deactivateId"];

	reverbProperties = new FMOD_REVERB_PROPERTIES();
	if (FIND(cfg_, "decayTime")) reverbProperties->DecayTime = cfg_["decayTime"];
	else  reverbProperties->DecayTime = 1500.0f;
	if (FIND(cfg_, "earlyDelay")) reverbProperties->EarlyDelay = cfg_["earlyDelay"];
	else reverbProperties->EarlyDelay = 7.0f;
	if (FIND(cfg_, "lateDelay")) reverbProperties->LateDelay = cfg_["lateDelay"];
	else  reverbProperties->LateDelay = 11.0f;
	if (FIND(cfg_, "hfReference")) reverbProperties->HFReference = cfg_["hfReference"];
	else  reverbProperties->HFReference = 5000.0f;
	if (FIND(cfg_, "hfDecayRatio")) reverbProperties->HFDecayRatio = cfg_["hfDecayRatio"];
	else  reverbProperties->HFDecayRatio = 10.0f;
	if (FIND(cfg_, "diffusion")) reverbProperties->Diffusion = cfg_["diffusion"];
	else  reverbProperties->Diffusion = 100.0f;
	if (FIND(cfg_, "density")) reverbProperties->Density = cfg_["density"];
	else  reverbProperties->Density = 100.0f;
	if (FIND(cfg_, "lowShelfFrecuency")) reverbProperties->LowShelfFrequency = cfg_["lowShelfFrecuency"];
	else  reverbProperties->LowShelfFrequency = 250.0f;
	if (FIND(cfg_, "lowShelfGain")) reverbProperties->LowShelfGain = cfg_["lowShelfGain"];
	else  reverbProperties->LowShelfGain = 0.0f;
	if (FIND(cfg_, "highCut")) reverbProperties->HighCut = cfg_["highCut"];
	else  reverbProperties->HighCut = 500.0f;
	if (FIND(cfg_, "earlyLateMix")) reverbProperties->EarlyLateMix = cfg_["earlyLateMix"];
	else  reverbProperties->EarlyLateMix = 92.0f;
	if (FIND(cfg_, "wetLevel")) reverbProperties->WetLevel = cfg_["wetLevel"];
	else  reverbProperties->WetLevel = 7.0f;
}

void ReverbComponent::lateSetUp()
{
	SoundManager::getSingleton()->getEngine()->createReverb3D(&reverb);
	reverb->set3DAttributes(&position.fmod(), minDist, maxDist);
}

void ReverbComponent::receive(Message * msg)
{
	if (msg->id_ == (MessageId)activateId) {
		reverb->setProperties(reverbProperties);
	}
	else if (msg->id_ == (MessageId)deactivateId) {	
		FMOD_REVERB_PROPERTIES defaultProps = FMOD_PRESET_OFF;
		reverb->setProperties(&defaultProps);
	}
}

#include "GOFactory.h"
REGISTER_TYPE(ReverbComponent);

