#pragma once
#include "tod/object.h"
#include "tod/objref.h"
namespace tod::graphics
{

class Shader;
class Renderer;

class ShaderCache : public SingletonDerive<ShaderCache, Object>
{
public:
    typedef ObjRef<Shader> ShaderRef;
    typedef std::unordered_map<int, ShaderRef> Shaders;

public:
    ShaderCache();
    
    Shader* getShader(const String& vsfname, const String& psfname);
    
private:
    int make_hash(const String& vsfname, const String& psfname);
    
private:
    ObjRef<Renderer> renderer;
    Shaders shaders;
};

}
