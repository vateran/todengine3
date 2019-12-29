#pragma once
#include "tod/graphics/matrix44.h"
#include "tod/graphics/mesh.h"
namespace tod::graphics
{

class Bone;
class BoneAnim;
    
class SkinMesh : public Derive<SkinMesh, Mesh>
{
public:
    SkinMesh();
    virtual~SkinMesh();

};

}