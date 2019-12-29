#include "tod/graphics/meshcomponent.h"
#include "tod/timemgr.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/meshloader.h"
#include "tod/graphics/boneanimator.h"
#include "tod/graphics/constantbuffer.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
MeshComponent::MeshComponent()
{
}


//-----------------------------------------------------------------------------
MeshComponent::~MeshComponent()
{
}


//-----------------------------------------------------------------------------
void MeshComponent::render(Shader* shader, uint32 passes)
{
    if (this->mesh.invalid()) return;
    
    this->mesh->render(shader, passes);
}


//-----------------------------------------------------------------------------
void MeshComponent::setUri(const String& uri)
{
    this->uri = uri;
    if (this->uri.empty()) return;

    Mesh* loaded_mesh = nullptr;
    if (!MeshLoader::instance()->load(uri, &loaded_mesh))
    {
        TOD_RETURN_TRACE();
    }

    this->mesh = loaded_mesh;
    this->setAABB(this->mesh->getAABB());
}


//-----------------------------------------------------------------------------
const String& MeshComponent::getUri()
{
    return this->uri;
}


//-----------------------------------------------------------------------------
void MeshComponent::bindProperty()
{
    BIND_PROPERTY(const String&, "uri", "불러올 mesh 파일 uri", setUri, getUri, "", PropertyAttr::DEFAULT);
    get_type()->findProperty("uri")->setEditorType(Property::EDITORTYPE_FILEDIALOG);
}

}
