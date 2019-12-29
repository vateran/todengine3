#include "tod/graphics/boneanim.h"
#include "tod/graphics/boneanimtrack.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
BoneAnim::BoneAnim(const String& anim_name, float duration)
: name(anim_name)
, duration(duration)
{
}


//-----------------------------------------------------------------------------
BoneAnim::~BoneAnim()
{
    for (auto i : this->boneAnimTracks)
    {
        TOD_SAFE_DELETE(i.second);
    }
    this->duration = 0;
    this->boneAnimTracks.clear();
}


//-----------------------------------------------------------------------------
BoneAnimTrack* BoneAnim::createBoneAnimTrack(const String& name)
{
    auto i = this->boneAnimTracks.find(name.hash());
    if (this->boneAnimTracks.end() != i)
    {
        return i->second;
    }

    auto bone_anim_track = new BoneAnimTrack();
    this->boneAnimTracks.emplace(name.hash(), bone_anim_track);
    return bone_anim_track;
}


//-----------------------------------------------------------------------------
BoneAnimTrack* BoneAnim::findBoneAnimTrack(const String& name)
{
    auto i = this->boneAnimTracks.find(name.hash());
    if (this->boneAnimTracks.end() == i)
    {
        return nullptr;
    }

    return i->second;
}


//-----------------------------------------------------------------------------
void BoneAnim::update(float elapsed)
{
    for (auto& i : this->boneAnimTracks)
    {
        i.second->update(elapsed);
    }
}

}