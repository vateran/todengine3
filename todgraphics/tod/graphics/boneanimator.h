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

    ///@지정된 이름(anim_name)의 BoneAnim 을 생성하여 반환(있으면 있는거 반환)
    BoneAnim* newBoneAnim(const String& anim_name, float duration);
    ///@지정된 이름(anim_name)의 BoneAnim 를 찾아서 반환
    BoneAnim* findBoneAnim(const String& anim_name);
    ///@지정된 이름(bone_name)의 Animation 을 실행
    void play(const String& anim_name);

private:
    //BoneAnim 리스트
    typedef int32 BoneAnimNameHash;
    typedef std::unordered_map<BoneAnimNameHash, BoneAnim*> BoneAnims;
    BoneAnims boneAnim;

    Matrix44 globalInvTransform;
    BoneAnim* curBoneAnim;
    float elapsed;

    friend class TodFileConverter;
};

}