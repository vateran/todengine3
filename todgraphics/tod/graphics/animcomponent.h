#pragma once
#include "tod/graphics/behaviorcomponent.h"
namespace tod::graphics
{

class Skeleton;
class BoneAnimator;

class AnimComponent : public Derive<AnimComponent, BehaviorComponent>
{
public:
    AnimComponent();
    virtual~AnimComponent();

    void update() override;

    void setAnimUri(const String& uri);
    const String& getAnimUri();
    void setSkeletonUri(const String& uri);
    const String& getSkeletonUri();

    void setAnimName(const String& name);
    const String& getAnimName();

    static void bindProperty();

private:
    String animUri;
    String skeletonUri;
    String animName;
    BoneAnimator* boneAnimator;
    Skeleton* skeleton;
};

}