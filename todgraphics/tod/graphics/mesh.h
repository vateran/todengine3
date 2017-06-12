#pragma once
#include "tod/object.h"
namespace tod::graphics
{
    
class Mesh : public Derive<Mesh, Object>
{
public:
    Mesh() {}
    virtual~Mesh() {}
    
    virtual void render()=0;
    virtual void load()=0;
    virtual void unload()=0;

};

}
