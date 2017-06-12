#pragma once
#include "tod/object.h"
namespace tod::graphics
{

class Shader : public Derive<Shader, Object>
{
public:
    Shader() {}
    virtual~Shader() {}
    
    virtual bool begin()=0;
    virtual void end()=0;
    virtual bool load(const String& vshader_fname, const String& fshader_fname)=0;
    virtual void unload()=0;
};

}
