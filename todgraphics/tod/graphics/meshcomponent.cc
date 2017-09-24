#include "tod/graphics/meshcomponent.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/texturecache.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
MeshComponent::MeshComponent()
: mesh(nullptr)
, renderer("/sys/renderer")
{
    this->mesh = this->renderer->createMesh();
}


//-----------------------------------------------------------------------------
MeshComponent::~MeshComponent()
{
}


//-----------------------------------------------------------------------------
void MeshComponent::updateShaderParams(Shader* shader)
{
    static Texture* s_empty_diffuse =
        TextureCache::instance()->getTexture("empty_diffuse");
    static Texture* s_empty_normal =
        TextureCache::instance()->getTexture("empty_normal");
    static Texture* s_empty_specular =
        TextureCache::instance()->getTexture("empty_specular");
    this->set_texture(shader, "DiffuseTex", 0, s_empty_diffuse);
    this->set_texture(shader, "SpecularTex", 1, s_empty_normal);
    this->set_texture(shader, "NormalTex", 2, s_empty_specular);
}


//-----------------------------------------------------------------------------
void MeshComponent::set_texture
(Shader* shader, const char* map, int sampler_index, Texture* empty)
{
    auto i = this->textures.find(map);
    if (this->textures.end() == i)
    {
        empty->use(sampler_index);
    }
    else
    {
        auto& info = i->second;
        info.texture->use(sampler_index);
    }
}


//-----------------------------------------------------------------------------
void MeshComponent::render()
{
    if (this->mesh.invalid()) return;
    
    this->mesh->render();
}


//-----------------------------------------------------------------------------
void MeshComponent::setTexture(const String& type, const String& uri)
{
    auto texture = TextureCache::instance()->getTexture(uri);
    if (nullptr == texture) TOD_RETURN_TRACE();

    TextureInfo info;
    info.type = type;
    info.uri = uri;
    info.texture = texture;
    this->textures[type] = info;
}

}
