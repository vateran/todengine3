#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/shadercache.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
ShaderComponent::ShaderComponent():
  shader(nullptr)
, renderer("/sys/renderer")
{
}


//-----------------------------------------------------------------------------
ShaderComponent::~ShaderComponent()
{
}


//-----------------------------------------------------------------------------
void ShaderComponent::begin()
{
    if (this->shader.invalid()) return;
    this->shader->begin();
}


//-----------------------------------------------------------------------------
void ShaderComponent::end()
{
    if (this->shader.invalid()) return;
    this->shader->end();
}


//-----------------------------------------------------------------------------
void ShaderComponent::setVShaderFileName(const String& fname)
{
    this->vshaderFileName = fname;
    
    this->load();
}


//-----------------------------------------------------------------------------
const String& ShaderComponent::getVShaderFileName()
{
    return this->vshaderFileName;
}


//-----------------------------------------------------------------------------
void ShaderComponent::setPShaderFileName(const String& fname)
{
    this->pshaderFileName = fname;
    
    this->load();
}


//-----------------------------------------------------------------------------
const String& ShaderComponent::getPShaderFileName()
{
    return this->pshaderFileName;
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
void ShaderComponent::bindProperty()
{
    BIND_PROPERTY(const String&, "vshader_fname", "",
        setVShaderFileName, getVShaderFileName, "", PropertyAttr::DEFAULT);
    BIND_PROPERTY(const String&, "pshader_fname", "",
        setPShaderFileName, getPShaderFileName, "", PropertyAttr::DEFAULT);
}


//-----------------------------------------------------------------------------
bool ShaderComponent::load()
{
    if (this->vshaderFileName.empty() || this->pshaderFileName.empty())
        return false;
    this->shader = ShaderCache::instance()->getShader(
        this->vshaderFileName,
        this->pshaderFileName);
    return this->shader != nullptr;
}

}
