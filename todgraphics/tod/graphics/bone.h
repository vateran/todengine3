#pragma once
#include <map>
#include <unordered_map>
#include "tod/string.h"
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

class Bone
{
public:
    typedef std::vector<Bone*> Bones;

public:
    Bone(const String& name, int32 bone_transform_index);
    ~Bone();

    void addChild(Bone* bone);

    void setName(const String& name) { this->name = name; }
    const String& getName() const { return this->name; }

    const Bones& getChildren() const { return this->children; }
    int32 getBoneTransformIndex() const { return this->boneTransformIndex; }
    const Matrix44& getBoneOffset() const { return this->boneOffset; }
    const Matrix44& getBindTransform() const { return this->bindTransform; }

private:
    String name;
    int32 boneTransformIndex;
    Bone* parent;
    Bones children;
    Matrix44 boneOffset;
    Matrix44 bindTransform;
};

}