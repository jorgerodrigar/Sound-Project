#include "Transforms.h"

//maybe better macros?

#include <OgreVector3.h>
//inline nap_vector3::nap_vector3(Ogre::Vector3 const * v)
//	: x_(v.x), y_(v.y), z_(v.z) {};

Ogre::Vector3 nap_vector3::ogre() {
	return Ogre::Vector3(x_, y_, z_);
}

#include <OgreQuaternion.h>
//inline nap_quat::nap_quat(Ogre::Quaternion const * v)
//	: w_(v.w), x_(v.x), y_(v.y), z_(v.z) {};

Ogre::Quaternion nap_quat::ogre() {
	return Ogre::Quaternion(w_, x_, y_, z_);
}


#include <PxRigidActor.h>
//inline nap_vector3::nap_vector3(physx::PxVec3 const * v)
//	: x_(v.x), y_(v.y), z_(v.z) {};
//inline nap_quat::nap_quat(physx::PxQuat const * v)
//	: w_ (v.w), x_(v.x), y_(v.y), z_(v.z) {};

physx::PxVec3 nap_vector3::px() {
	return physx::PxVec3(x_, y_, z_);
}
physx::PxQuat nap_quat::px() {
	return physx::PxQuat(x_, y_, z_, w_);
}
nap_vector3 nap_quat::toNapVec3(nap_vector3 direction) {
	Ogre::Vector3 aux = ogre() * direction.ogre();
	return nap_vector3(aux.x, aux.y, aux.z).normalize();
}
#include <characterkinematic\PxExtended.h>
physx::PxExtendedVec3 nap_vector3::pxEx() {
	return physx::PxExtendedVec3(x_, y_, z_);
}

#include <fmod.hpp>
FMOD_VECTOR nap_vector3::fmod()
{
	FMOD_VECTOR fmodVec; fmodVec.x = x_; fmodVec.y = y_; fmodVec.z = z_;
	return fmodVec;
}
