#pragma once
#include <map>
#include <tuple>
#include "tod/objref.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/rendertarget.h"
namespace tod::graphics
{

class Renderer;
class RenderTargetTexturePool :
public SingletonDerive<RenderTargetTexturePool, Object>
{
public:
    RenderTargetTexturePool();

    Texture* getTexture(int32 width, int32 height, Format format);
    void release(Texture* texture);
    
private:
    typedef std::tuple<int, int, Format> Key;
    typedef std::map<Key, ObjRef<Texture>> RTTextures;
    RTTextures rtTextures;
    ObjRef<Renderer> renderer;
};

}
