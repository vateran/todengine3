#pragma once
#include "tod/component.h"
namespace tod::graphics
{

class Shader;
class RenderComponent : public Derive<RenderComponent, Component>
{
public:
    virtual void updateShaderParams(Shader* shader)=0;
    virtual void render()=0;
};

}
