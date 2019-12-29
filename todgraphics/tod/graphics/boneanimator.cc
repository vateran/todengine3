#include "tod/graphics/boneanimator.h"
#include "tod/graphics/boneanim.h"
#include "tod/graphics/boneanimtrack.h"
#include "tod/graphics/bone.h"
#include "tod/graphics/skeleton.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
BoneAnimator::BoneAnimator()
: curBoneAnim(nullptr)
, elapsed(0)
{
}


//-----------------------------------------------------------------------------
BoneAnimator::~BoneAnimator()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
BoneAnim* BoneAnimator::newBoneAnim(const String& anim_name, float duration)
{
    int32 name_hash = anim_name.hash();

    auto i = this->boneAnim.find(name_hash);
    if (this->boneAnim.end() != i)
    {
        return i->second;
    }

    auto bone_anim = new BoneAnim(anim_name, duration);
    this->boneAnim.emplace(name_hash, bone_anim);
    return bone_anim;
}


//-----------------------------------------------------------------------------
void BoneAnimator::destroy()
{
    for (auto i : this->boneAnim)
    {
        TOD_SAFE_DELETE(i.second);
    }
    this->boneAnim.clear();

    this->curBoneAnim = nullptr;
}


//-----------------------------------------------------------------------------
void BoneAnimator::update(float elapsed, Skeleton* skeleton)
{
    if (nullptr == this->curBoneAnim)
    {
        return;
    }

    this->elapsed = fmod(elapsed, this->curBoneAnim->duration);

    Matrix44 identity;
    identity.identity();

    this->curBoneAnim->update(this->elapsed);

    this->updateBoneTransform(skeleton, skeleton->getRootBone(), identity);
}


//-----------------------------------------------------------------------------
void BoneAnimator::updateBoneTransform
(Skeleton* skeleton, Bone* bone, const Matrix44& parent_transform)
{
    const Matrix44* node_tranform = &bone->getBindTransform();
    if (-1 != bone->getBoneTransformIndex())
    {
        //if (bone->getName() == "Bip01_Pelvis")
        {
            auto track = this->curBoneAnim->findBoneAnimTrack(bone->getName());
            if (nullptr != track)
            {
                node_tranform = &track->getLocalBoneTransform();
            }
        }
    }

    Matrix44 global_transform = parent_transform;
    global_transform *= (*node_tranform);

    //if (bone->getName() == "Bip01_Pelvis")
    {
        if (-1 != bone->getBoneTransformIndex())
        {
            Matrix44& final_transform
                = skeleton->getFinalBoneTransforms()[bone->getBoneTransformIndex()];
            final_transform = this->globalInvTransform;
            final_transform *= global_transform;
            final_transform *= bone->getBoneOffset();
        }
    }

    for (auto child_bone : bone->getChildren())
    {
        this->updateBoneTransform(skeleton, child_bone, global_transform);
    }
}


//-----------------------------------------------------------------------------
BoneAnim* BoneAnimator::findBoneAnim(const String& anim_name)
{
    auto i = this->boneAnim.find(anim_name.hash());
    if (this->boneAnim.end() == i)
    {
        TOD_RETURN_TRACE(nullptr);
    }

    return i->second;
}


//-----------------------------------------------------------------------------
void BoneAnimator::play(const String& anim_name)
{
    auto bone_anim = this->findBoneAnim(anim_name);
    if (nullptr == bone_anim)
    {
        TOD_RETURN_TRACE();
    }

    this->curBoneAnim = bone_anim;
    this->elapsed = 0;
}

}