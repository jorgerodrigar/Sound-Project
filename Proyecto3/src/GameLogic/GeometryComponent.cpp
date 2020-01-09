#include "GeometryComponent.h"
#include <fmod.hpp>
#include <SoundManager.h>

using namespace FMOD;

void GeometryComponent::setUp()
{
	if (FIND(cfg_, "maxPolygons")) maxPolygons = cfg_["maxPolygons"];
	else maxPolygons = 2;
	if (FIND(cfg_, "maxVertices")) maxVertices = cfg_["maxVertices"];
	else maxVertices = 5;

	if (FIND(cfg_, "vertices")) {
		for (int i = 0; i < maxVertices; i++) {
			vertices.push_back(nap_vector3(cfg_["vertices"][i]));
		}
	}

	if (FIND(cfg_, "orientation"))orientation = nap_vector3(cfg_["orientation"]);
	orientation = nap_vector3(0, 0, 1);
	if (FIND(cfg_, "position"))position = nap_vector3(cfg_["position"]);
	position = nap_vector3(0, -1, 0);

	if (FIND(cfg_, "occlusion")) occlusion = cfg_["occlusion"];
	else occlusion = 1.0;
	if (FIND(cfg_, "reverbOcclusion")) reverbOcclusion = cfg_["reverbOcclusion"];
	else reverbOcclusion = 1.0;
	if (FIND(cfg_, "bothSides")) bothSides = cfg_["bothSides"];
	else bothSides = true;
}

void GeometryComponent::lateSetUp()
{
	SoundManager::getSingleton()->getEngine()->createGeometry(maxPolygons, maxVertices, &geometry);
	int polygonIndex;
	FMOD_VECTOR* fmodVertices = new FMOD_VECTOR[maxVertices];
	for (int i = 0; i < maxVertices; i++) {
		fmodVertices[i] = vertices[i].fmod();
	}

	geometry->addPolygon(occlusion, reverbOcclusion, bothSides, maxVertices, fmodVertices, &polygonIndex);

	FMOD_VECTOR geometryPos = position.fmod();
	FMOD_VECTOR geometryForward = orientation.fmod();
	FMOD_VECTOR geometryUp = { 0,1,0 };
	geometry->setPosition(&geometryPos);
	geometry->setRotation(&geometryForward, &geometryUp);

	delete fmodVertices; fmodVertices = nullptr;
}

#include "GOFactory.h"
REGISTER_TYPE(GeometryComponent);

