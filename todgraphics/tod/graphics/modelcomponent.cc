#include "tod/graphics/modelcomponent.h"
#include "tod/graphics/fbxloader.h"
#include "tod/graphics/modelloader.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
ModelComponent::ModelComponent()
{
}


//-----------------------------------------------------------------------------
ModelComponent::~ModelComponent()
{
}


//-----------------------------------------------------------------------------
void ModelComponent::setUri(const String& uri)
{
    this->uri = uri;
    if (this->uri.empty())
    {
        meshNode.release();
        return;
    }
    
    if (this->meshNode.valid())
        this->meshNode->removeFromParent();
    
    
    this->meshNode = ModelLoader::instance()->load(this->uri);
    if (this->meshNode.invalid()) return;
    this->meshNode->setSerializable(false);
    
    //this->meshNode->setHidden(true);
    this->getNode()->addChild(this->meshNode);
}


//-----------------------------------------------------------------------------
const String& ModelComponent::getUri()
{
    return this->uri;
}


//-----------------------------------------------------------------------------
void ModelComponent::bindProperty()
{
    BIND_PROPERTY(const String&, "uri", "uri", setUri, getUri, String(), PropertyAttr::DEFAULT);
    SelfType::get_type()->findProperty("uri")->setEditorType(Property::EDITORTYPE_FILEDIALOG);
}


}
