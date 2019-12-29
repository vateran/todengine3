#include "tod/graphics/bone.h"



#include "tod/graphics/boneanimator.h"


namespace tod::graphics
{

//-----------------------------------------------------------------------------
Bone::Bone(const String& name, int32 bone_transform_index)
: name(name)
, boneTransformIndex(bone_transform_index)
, parent(nullptr)
{
    this->bindTransform.identity();
    this->boneOffset.identity();
}


//-----------------------------------------------------------------------------
Bone::~Bone()
{
    for (auto bone : this->children)
    {
        TOD_SAFE_DELETE(bone);
    }
}


//-----------------------------------------------------------------------------
void Bone::addChild(Bone* bone)
{
    bone->parent = this;
    this->children.emplace_back(bone);
}

}