#pragma once
#include "tod/object.h"
#include "tod/objref.h"
namespace tod::graphics
{

class Shader;

class ShaderCache : public SingletonDerive<ShaderCache, Object>
{
public:
    typedef ObjRef<Shader> ShaderRef;
    typedef std::unordered_map<int32, ShaderRef> Shaders;

public:
    ShaderCache();
    
    Shader* getShader(const String& shader_fname);
    
private:
    Shaders shaders;
};

}
