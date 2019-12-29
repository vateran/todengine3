#include "tod/graphics/heightfieldterraincomponent.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/mesh.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
HeightFieldTerrainComponent::HeightFieldTerrainComponent()
: mesh(nullptr)
, renderer("/sys/renderer")
, width(0)
, height(0)
{
    this->mesh = this->renderer->createMesh();
}
 

//-----------------------------------------------------------------------------
void HeightFieldTerrainComponent::updateShaderParams(Shader* shader)
{
}


//-----------------------------------------------------------------------------
void HeightFieldTerrainComponent::render(Shader* shader)
{
    if (this->mesh.invalid()) return;
    
    this->mesh->render();
}


//-----------------------------------------------------------------------------
void HeightFieldTerrainComponent::rebuild()
{
    if (0 >= this->width || 0 >= this->height) return;
    
    this->mesh->unload();
    
    this->mesh->setupVertexStruct(VertexDataOption()
            .add(VertexDataType::POSITION)
            .add(VertexDataType::NORMAL)
            .add(VertexDataType::TANGENT)
            .add(VertexDataType::TEXCOORD));
    this->mesh->lock(this->width * this->height);
    this->mesh->unlock();
}


//-----------------------------------------------------------------------------
void HeightFieldTerrainComponent::setHeightFieldFileName(const char* fname)
{
    this->heightFieldFName = fname;
    
    ObjRef<Texture> hf_tex = this->renderer->createTexture();
    if (!hf_tex->load("heightfield.jpg")) return;
}


//-----------------------------------------------------------------------------
void HeightFieldTerrainComponent::bindProperty()
{
    BIND_PROPERTY(int, "width", "width", setWidth, getWidth, 0, PropertyAttr::DEFAULT);
    BIND_PROPERTY(int, "height", "height", setHeight, getHeight, 0, PropertyAttr::DEFAULT);
}

}
