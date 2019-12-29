#pragma once
#include "tod/object.h"
#include "tod/objref.h"
#include "tod/staticstring.h"
#include "tod/graphics/texture.h"
namespace tod::graphics
{

class TextureCache : public SingletonDerive<TextureCache, Object>
{
public:
    typedef ObjRef<Texture> TextureRef;
    typedef std::unordered_map<StaticString, TextureRef> Textures;
    
public:
    TextureCache();

    Texture* getTexture(const StaticString& uri);
    
private:
    Textures textures;
    
};

}
