#include "tod/graphics/shadercache.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/shader.h"
namespace tod::graphics
{


//-----------------------------------------------------------------------------
ShaderCache::ShaderCache():
renderer("/sys/renderer")
{
}


//-----------------------------------------------------------------------------
Shader* ShaderCache::getShader
(const String& vsfname, const String& psfname)
{
    auto hash = this->make_hash(vsfname, psfname);
    auto i = this->shaders.find(hash);
    if (this->shaders.end() == i)
    {
        auto shader = this->renderer->createShader();
        if (!shader->load(vsfname, psfname))
            TOD_RETURN_TRACE(nullptr);
        this->shaders[hash] = shader;
        return shader;
    }
    return i->second;
}


//-----------------------------------------------------------------------------
int ShaderCache::make_hash(const String& vsfname, const String& psfname)
{
    String s = vsfname;
    s += psfname;
    return s.hash();
}

}
