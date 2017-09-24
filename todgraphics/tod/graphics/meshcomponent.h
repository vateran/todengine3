#pragma once
#include <map>
#include "tod/graphics/rendercomponent.h"
namespace tod::graphics
{

class Mesh;
class Renderer;
class Texture;
class MeshComponent : public Derive<MeshComponent, RenderComponent>
{
public:
    MeshComponent();
    virtual~MeshComponent();
    
    void updateShaderParams(Shader* shader) override;
    void render() override;
    void setTexture(const String& type, const String& uri);
    inline Mesh* getMesh() { return this->mesh; }
    
private:
    void set_texture(Shader* shader, const char* map, int sampler_index, Texture* empty);
    
private:
    ObjRef<Mesh> mesh;
    ObjRef<Renderer> renderer;
    
    struct TextureInfo
    {
        String type;
        String uri;
        ObjRef<Texture> texture;
    };
    std::map<String, TextureInfo> textures;
};

}
