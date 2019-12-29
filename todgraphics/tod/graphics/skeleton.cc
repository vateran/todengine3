#include "tod/graphics/skeleton.h"
#include "tod/graphics/bone.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
Skeleton::Skeleton()
    : root(nullptr)
{
}


//-----------------------------------------------------------------------------
Bone* Skeleton::newBone(const String& bone_name, bool is_bone) const
{
    auto i = this->bones.find(bone_name.hash());
    if (this->bones.end() == i)
    {
        Bone* bone = new Bone(
            bone_name, is_bone
            ? static_cast<int32>(this->finalBoneTransforms.size()) : -1);
        this->bones.emplace(bone_name.hash(), bone);
        if (true == is_bone)
        {
            this->finalBoneTransforms.emplace_back(Matrix44(true));
        }
        if (nullptr == this->root) { this->root = bone; }
        return bone;
    }
    return i->second;
}


//-----------------------------------------------------------------------------
bool Skeleton::retargeting
(BoneAnimator* bone_animator, const std::map<String, String>& m)
{
    Bones new_bones;

    for (auto& b : this->bones)
    {
        Bone* bone = b.second;
        auto i = m.find(bone->getName());
        if (m.end() == i)
        {
            new_bones.emplace(bone->getName().hash(), bone);
            continue;
        }

        printf("Changed[%s]->[%s]\n", bone->getName().c_str(), i->second.c_str());
        new_bones.emplace(i->second.hash(), bone);
        bone->setName(i->second);
    }

    std::swap(this->bones, new_bones);

    return true;
}


}