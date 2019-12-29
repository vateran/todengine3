#pragma once
#include <unordered_map>
#include "tod/object.h"
#include "tod/staticstring.h"
#include "tod/graphics/graphicsdef.h"
#include "tod/graphics/aabb.h"
namespace tod::graphics
{

class Shader;
class RenderInterface;
class VertexBuffer;
class IndexBuffer;

class Mesh : public Derive<Mesh, Object>
{
public:
    struct Material
    {
        enum
        {
            MAX_USE_TEXTURE = TextureType::LIGHT
        };
        StaticString textures[static_cast<int32>(MAX_USE_TEXTURE)];
    };
    typedef std::vector<Material> Materials;

    struct SubMesh
    {
    public:
        SubMesh(const char* name);
        ~SubMesh();

        void render(Shader* shader, uint32 passes);

        String name;
        VertexBuffer* vb;
        IndexBuffer* ib;
        uint32 materialIndex;
        AABB aabb;
    };
    typedef std::vector<SubMesh*> SubMeshes;

public:
    Mesh();
    virtual~Mesh();

    void destroy();

    SubMesh* newSubMesh(const char* name);

    inline void resizeMaterialSlot(uint32 count) { this->materials.resize(count); }
    inline Material* getMaterial(uint32 index) { return &this->materials[index]; }
    void updateAABB();
    inline const AABB& getAABB() const { return this->aabb; }

    virtual void render(Shader* shader, uint32 passes);

protected:
    Materials materials;
    SubMeshes subMeshes;    
    AABB aabb;
};

}
