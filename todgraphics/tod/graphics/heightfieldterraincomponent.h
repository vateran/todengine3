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
    
    void updateShaderParams(Shader* shader);
    void render(Shader* shader, uint32 passes) override;
    
    void setHeightFieldFileName(const char* fname);
    const char* getHeightFieldFileName()
    { return this->heightFieldFName.c_str(); }
    void setWidth(int32 value) { this->width = value; this->rebuild(); }
    int32 getWidth() { return this->width; }
    void setHeight(int32 value) { this->height = value; this->rebuild(); }
    int32 getHeight() { return this->height; }
    
    static void bindProperty();
    
private:
    void rebuild();
    
private:
    ObjRef<Renderer> renderer;
    ObjRef<Mesh> mesh;
    
    String heightFieldFName;
    int32 width;
    int32 height;
};

}
