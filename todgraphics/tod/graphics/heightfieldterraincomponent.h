#pragma once
#include "tod/graphics/rendercomponent.h"
namespace tod::graphics
{

class Renderer;
class Mesh;

class HeightFieldTerrainComponent
: public Derive<HeightFieldTerrainComponent, RenderComponent>
{
public:
    HeightFieldTerrainComponent();
    
    void updateShaderParams(Shader* shader) override;
    void render() override;
    
    void setHeightFieldFileName(const char* fname);
    const char* getHeightFieldFileName()
    { return this->heightFieldFName.c_str(); }
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
    
    String heightFieldFName;
    int width;
    int height;
};

}
