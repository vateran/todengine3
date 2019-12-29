#pragma once
#include "tod/string.h"
#include "tod/object.h"
namespace tod::graphics
{

class Mesh;
class BoneAnimator;
class AnimLoader;

class MeshLoader : public SingletonDerive<MeshLoader, Object>
{
public:
    MeshLoader();
    virtual~MeshLoader();

    bool load(const String& uri, Mesh** mesh);

private:
    class MeshLoaderPrivate;
    MeshLoaderPrivate* inner;
    friend AnimLoader;
};


class AnimLoader : public SingletonDerive<AnimLoader, Object>
{
public:
    AnimLoader();
    virtual~AnimLoader();

    bool load(const String& uri, BoneAnimator** bone_animator);

private:
    MeshLoader::MeshLoaderPrivate* inner;
};


}