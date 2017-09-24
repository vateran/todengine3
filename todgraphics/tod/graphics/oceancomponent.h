#pragma once
#include "tod/graphics/rendercomponent.h"
namespace tod::graphics
{

class Renderer;
class Mesh;

class OceanComponent : public Derive<OceanComponent, RenderComponent>
{
public:
    OceanComponent();
    
    void updateShaderParams(Shader* shader) override;
    void render() override;
    
    void setWidth(int value) { this->width = value; this->rebuild(); }
    int getWidth() { return this->width; }
    void setHeight(int value) { this->height = value; this->rebuild(); }
    int getHeight() { return this->height; }
    
    static void bindProperty();
    
private:
    void rebuild();
    
private:
    ObjRef<Renderer> renderer;
    ObjRef<Mesh> mesh;
    
    int width;
    int height;
};

}
