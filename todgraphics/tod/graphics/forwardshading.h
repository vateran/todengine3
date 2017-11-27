#pragma once
#include "tod/objref.h"
#include "tod/graphics/shading.h"
namespace tod::graphics
{

class Shader;

class ForwardShading : public Shading
{
public:
    ForwardShading();
    friend class Renderer;
    
    void init(Renderer* renderer, int width, int height) override;
    void render() override;
    void render_cameras();
    
private:
    int width;
    int height;
    Renderer* renderer;
    
    ObjRef<Shader> lightShader;
};

}
