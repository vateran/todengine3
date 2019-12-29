#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/shadercache.h"
#include "tod/graphics/inputlayout.h"
#include "tod/graphics/inputlayoutcache.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
ShaderComponent::ShaderComponent()
: techniqueIndex(-1)
, inputLayout(nullptr)
{
}


//-----------------------------------------------------------------------------
ShaderComponent::~ShaderComponent()
{
}


//-----------------------------------------------------------------------------
void ShaderComponent::begin(uint32& passes)
{
    if (this->shader.invalid()) return;

    if (nullptr != this->inputLayout)
    {
        this->inputLayout->use();
    }
    
    this->shader->begin(this->techniqueIndex, passes);
}


//-----------------------------------------------------------------------------
void ShaderComponent::end()
{
    if (this->shader.invalid()) return;
    this->shader->end();
}


//-----------------------------------------------------------------------------
bool ShaderComponent::setParam(const String& name, const std::any& value)
{
    if (this->shader.invalid()) TOD_RETURN_TRACE(false);
    return this->shader->setParam(name, value);
}


//-----------------------------------------------------------------------------
bool ShaderComponent::commitParams()
{
    if (this->shader.invalid()) TOD_RETURN_TRACE(false);
    return this->shader->commitParams();
}


//-----------------------------------------------------------------------------
void ShaderComponent::setUri(const String& uri)
{
    if (uri.empty()) return;
    if (this->uri == uri) return;
    this->uri = uri;
    if (this->uri.empty()) return;
    
    this->shader = ShaderCache::instance()->getShader(this->uri);
}


//-----------------------------------------------------------------------------
const String& ShaderComponent::getUri()
{
    return this->uri;
}


//-----------------------------------------------------------------------------
void ShaderComponent::setTechnique(const String& name)
{
    if (this->shader.invalid()) TOD_RETURN_TRACE();
    if (this->techniqueName == name) return;
    if (name.empty()) return;

    this->techniqueName = name;
    this->techniqueIndex = this->shader->getTechniqueIndex(name);
    this->inputLayout = InputLayoutCache::instance()->getInputLayout(name);
}


//-----------------------------------------------------------------------------
const String& ShaderComponent::getTechnique()
{
    return this->techniqueName;
}


//-----------------------------------------------------------------------------
Shader* ShaderComponent::getShader()
{
    return this->shader;
}


//-----------------------------------------------------------------------------
void ShaderComponent::bindProperty()
{
    BIND_PROPERTY(const String&, "uri", "", setUri, getUri, "", PropertyAttr::DEFAULT);
    BIND_PROPERTY(const String&, "technique", "", setTechnique, getTechnique, "", PropertyAttr::DEFAULT);
}

}

