#pragma once
namespace tod::graphics
{
class Renderer;
class Shading
{
public:
    virtual void init(int32 width, int32 height)=0;
    virtual void render()=0;
};

}
