#pragma once
#include "tod/object.h"
#include "tod/objref.h"
namespace tod::graphics
{

class Renderer;
class Texture;

class TextureCache : public SingletonDerive<TextureCache, Object>
{
public:
    typedef ObjRef<Texture> TextureRef;
    typedef std::unordered_map<int, TextureRef> Textures;
    
public:
    TextureCache();

    Texture* getTexture(const String& uri);
    
private:
    ObjRef<Renderer> renderer;
    Textures textures;
    
};

}
