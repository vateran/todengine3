#include "tod/graphics/oceancomponent.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/mesh.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
OceanComponent::OceanComponent()
: mesh(nullptr)
, renderer("/sys/renderer")
, width(0)
, height(0)
{
    this->mesh = this->renderer->createMesh();
}
 

//-----------------------------------------------------------------------------
void OceanComponent::updateShaderParams(Shader* shader)
{
}


//-----------------------------------------------------------------------------
void OceanComponent::render()
{
    if (this->mesh.invalid()) return;
    
    this->mesh->render();
}


//-----------------------------------------------------------------------------
void OceanComponent::rebuild()
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
void OceanComponent::bindProperty()
{
    BIND_PROPERTY(int, "width", "width", setWidth, getWidth, 0, PropertyAttr::DEFAULT);
    BIND_PROPERTY(int, "height", "height", setHeight, getHeight, 0, PropertyAttr::DEFAULT);
}

}
