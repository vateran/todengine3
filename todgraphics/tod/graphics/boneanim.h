#pragma once
#include <unordered_map>
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

class BoneAnimTrack;

class BoneAnim
{
public:
    BoneAnim(const String& anim_name, float duration);
    ~BoneAnim();
    
    void update(float elapsed);

    BoneAnimTrack* createBoneAnimTrack(const String& name);
    BoneAnimTrack* findBoneAnimTrack(const String& name);

public:
    typedef int32 BoneNameHash;
    typedef std::unordered_map<BoneNameHash, BoneAnimTrack*> BoneAnimTracks;

    String name;
    float duration;
    BoneAnimTracks boneAnimTracks;
};

}