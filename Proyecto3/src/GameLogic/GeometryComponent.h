//Nap_Time_Studios

#ifndef GEOMETRY__COMP_H_
#define GEOMETRY__COMP_H_

#include "Component.h"
#include <vector>

namespace FMOD {
	class Geometry;
}

class GeometryComponent : public Component
{
private:
	FMOD::Geometry* geometry;

	int maxPolygons, maxVertices;
	vector<nap_vector3> vertices;
	nap_vector3 position;
	nap_vector3 orientation;

	float occlusion, reverbOcclusion;
	bool bothSides;

public:
	inline GeometryComponent(nap_json const & cfg, GameObject* owner) : Component(cfg, owner) {};
	inline virtual ~GeometryComponent() {}
	virtual void setUp();
	virtual void lateSetUp();
};

#endif /* GEOMETRY__COMP_H_ */

