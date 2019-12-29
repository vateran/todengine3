#include "tod/graphics/boneanimtrack.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
void BoneAnimTrack::update(float elapsed)
{
    Vector3 t, s;
    Quaternion r;

    this->find_key<Keys, Vector3>(elapsed, this->positionKeys, t);
    this->find_key<QuatKeys, Quaternion>(elapsed, this->rotateKeys, r);
    this->find_key<Keys, Vector3>(elapsed, this->scaleKeys, s);
    r.normalize();

    this->localBoneTransform.makeFromSRTTranspose(s, r, t);
}

}