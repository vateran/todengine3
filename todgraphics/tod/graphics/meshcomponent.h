#pragma once
#include "tod/graphics/rendercomponent.h"
namespace tod::graphics
{

class Mesh;
class Texture;
class RenderInterface;

class MeshComponent : public Derive<MeshComponent, RenderComponent>
{
public:
    MeshComponent();
    virtual~MeshComponent();

    void render(Shader* shader, uint32 passes) override;
    inline Mesh* getMesh() { return this->mesh; }

    void setUri(const String& uri);
    const String& getUri();

    static void bindProperty();
    
private:
    ObjRef<Mesh> mesh;
    String uri;
};

}
