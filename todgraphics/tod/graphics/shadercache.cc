#include "tod/graphics/shadercache.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/shader.h"
namespace tod::graphics
{


//-----------------------------------------------------------------------------
ShaderCache::ShaderCache()
{
}


//-----------------------------------------------------------------------------
Shader* ShaderCache::getShader(const String& shader_fname)
{
    int32 hash = shader_fname.hash();
    auto i = this->shaders.find(hash);
    if (this->shaders.end() == i)
    {
        Shader* shader = Renderer::instance()
            ->getRenderInterface()
            ->createShader();
        if (!shader->create(shader_fname, {}))
        {
            TOD_RETURN_TRACE(nullptr);
        }

        this->shaders[hash] = shader;

        return shader;
    }

    return i->second;
}


}
