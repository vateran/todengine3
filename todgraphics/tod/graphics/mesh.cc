#include "tod/graphics/mesh.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/texturecache.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/vertexbuffer.h"
#include "tod/graphics/indexbuffer.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
Mesh::SubMesh::SubMesh(const char* name)
: name(name)
, materialIndex(-1)
{
}


//-----------------------------------------------------------------------------
Mesh::SubMesh::~SubMesh()
{
    TOD_SAFE_DELETE(this->ib);
    TOD_SAFE_DELETE(this->vb);
}


//-----------------------------------------------------------------------------
void Mesh::SubMesh::render(Shader* shader, uint32 passes)
{   
    this->vb->use();
    this->ib->use();

    for (uint32 pass = 0; pass < passes; ++pass)
    {
        shader->beginPass(pass);
        Renderer::instance()
            ->getRenderInterface()
            ->drawPrimitive(this->ib->getIndexCount());
        shader->endPass();
    }
}


//-----------------------------------------------------------------------------
Mesh::Mesh()
{
    TOD_ASSERT(Renderer::instance()->isValid(), "Renderer 생성필요");
}


//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
void Mesh::destroy()
{
    for (auto& sub_mesh : this->subMeshes)
    {
        TOD_SAFE_DELETE(sub_mesh);
    }
    this->subMeshes.clear();
}


//-----------------------------------------------------------------------------
Mesh::SubMesh* Mesh::newSubMesh(const char* name)
{
    auto sub_mesh = new SubMesh(name);
    sub_mesh->vb = Renderer::instance()
        ->getRenderInterface()
        ->createVertexBuffer();
    sub_mesh->ib = Renderer::instance()
        ->getRenderInterface()
        ->createIndexBuffer();
    this->subMeshes.emplace_back(sub_mesh);
    return sub_mesh;
}


//-----------------------------------------------------------------------------
void Mesh::updateAABB()
{   
    this->aabb.clear();
    for (auto& sub_mesh : this->subMeshes)
    {
        this->aabb.unionAABB(sub_mesh->aabb);
    }
}


//-----------------------------------------------------------------------------
void Mesh::render(Shader* shader, uint32 passes)
{
    //모든 SubMesh들을 그린다
    for (auto& sub_mesh : this->subMeshes)
    {
        //Shader에 Texture들을 셋팅
        if (-1 != sub_mesh->materialIndex)
        {
            Material* material = this->getMaterial(sub_mesh->materialIndex);
            for (int32 i = 0; i < Material::MAX_USE_TEXTURE; ++i)
            {
                if (material->textures[i].empty()) continue;
                auto texture = TextureCache::instance()->getTexture(material->textures[i]);
                if (nullptr == texture) continue;
                shader->setParam(TextureTypeDesc::repository[i].shaderParamName, texture);
            }
            shader->commitParams();
        }

        sub_mesh->render(shader, passes);
    }
}

}
