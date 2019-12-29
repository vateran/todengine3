#pragma once
#include <map>
#include <unordered_map>
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

class Bone;
class BoneAnimator;

class Skeleton
{
public:
    typedef std::vector<Matrix44> BoneTransforms;

public:
    Skeleton();

    Bone* newBone(const String& bone_name, bool is_bone) const;

    Bone* getRootBone() { return this->root; }
    BoneTransforms& getFinalBoneTransforms() { return this->finalBoneTransforms; }

    //@param m BoneName : AnimationBoneName ������ �̷���� map, Ư�� Bone�� �̸��� AnimationBoneName ���� �����Ѵ�
    bool retargeting(BoneAnimator* bone_animator, const std::map<String, String>& m);

private:
    typedef int32 BoneNameHash;
    typedef std::unordered_map<BoneNameHash, Bone*> Bones;
    mutable Bones bones;

    //���⿡ Animate �� ���� Matrix ������ �����
    mutable BoneTransforms finalBoneTransforms;  //-> Shader

    mutable Bone* root;
};

}