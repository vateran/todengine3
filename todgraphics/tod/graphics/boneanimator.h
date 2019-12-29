#pragma once
#include <map>
#include <set>
#include "tod/object.h"
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

class Bone;
class BoneAnim;
class Skeleton;

class BoneAnimator : public Derive<BoneAnimator, Object>
{
public:
    typedef int32 BoneTransformIndex;

public:
    BoneAnimator();
    ~BoneAnimator();
    
    void destroy();

    void update(float elapsed, Skeleton* skeleton);
    void updateBoneTransform(Skeleton* skeleton, Bone* bone, const Matrix44& parent_transform);

    ///@������ �̸�(anim_name)�� BoneAnim �� �����Ͽ� ��ȯ(������ �ִ°� ��ȯ)
    BoneAnim* newBoneAnim(const String& anim_name, float duration);
    ///@������ �̸�(anim_name)�� BoneAnim �� ã�Ƽ� ��ȯ
    BoneAnim* findBoneAnim(const String& anim_name);
    ///@������ �̸�(bone_name)�� Animation �� ����
    void play(const String& anim_name);

private:
    //BoneAnim ����Ʈ
    typedef int32 BoneAnimNameHash;
    typedef std::unordered_map<BoneAnimNameHash, BoneAnim*> BoneAnims;
    BoneAnims boneAnim;

    Matrix44 globalInvTransform;
    BoneAnim* curBoneAnim;
    float elapsed;

    friend class TodFileConverter;
};

}