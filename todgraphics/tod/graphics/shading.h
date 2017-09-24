#pragma once
namespace tod::graphics
{
class Renderer;
class Shading
{
public:
    virtual void init(Renderer* renderer, int width, int height)=0;
    virtual void render()=0;
};

}
