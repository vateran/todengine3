#include "tod/graphics/texturecache.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
TextureCache::TextureCache()
{   
    TOD_ASSERT(Renderer::instance()->isValid(), "Renderer 생성필요");

    {
        auto empty_diffuse = Renderer::instance()
            ->getRenderInterface()
            ->createTexture();
        uint32 data = 0x050505;
        empty_diffuse->create(1, 1, Format::R8G8B8A8, &data);
        empty_diffuse->setMinFilter(TextureFilter::LINEAR);
        empty_diffuse->setMagFilter(TextureFilter::LINEAR);
        empty_diffuse->setHorizontalWrap(TextureWrap::REPEAT);
        empty_diffuse->setVerticalWrap(TextureWrap::REPEAT);
        this->textures.insert(std::make_pair(
            StaticString("empty_diffuse"), empty_diffuse));
    }

    {
        auto empty_normal = Renderer::instance()
            ->getRenderInterface()
            ->createTexture();
        uint32 data = 0xff0000;
        empty_normal->create(1, 1, Format::R8G8B8A8, &data);
        empty_normal->setMinFilter(TextureFilter::LINEAR);
        empty_normal->setMagFilter(TextureFilter::LINEAR);
        empty_normal->setHorizontalWrap(TextureWrap::REPEAT);
        empty_normal->setVerticalWrap(TextureWrap::REPEAT);
        this->textures.insert(std::make_pair(
            StaticString("empty_normal"), empty_normal));
    }

    {
        auto empty_specular = Renderer::instance()
            ->getRenderInterface()
            ->createTexture();
        uint32 data = 0x111111;
        empty_specular->create(1, 1, Format::R8G8B8A8, &data);
        empty_specular->setMinFilter(TextureFilter::LINEAR);
        empty_specular->setMagFilter(TextureFilter::LINEAR);
        empty_specular->setHorizontalWrap(TextureWrap::REPEAT);
        empty_specular->setVerticalWrap(TextureWrap::REPEAT);
        this->textures.insert(std::make_pair(
            StaticString("empty_specular"), empty_specular));
    }
}


//-----------------------------------------------------------------------------
Texture* TextureCache::getTexture(const StaticString& uri)
{
    auto i = this->textures.find(uri);
    if (this->textures.end() == i)
    {
        //새로운 Texture를 만들고 로딩해본다
        Texture* new_tex = Renderer::instance()
            ->getRenderInterface()
            ->createTexture();
        if (!new_tex->load(uri.c_str()))
        {
            new_tex->release();
            return nullptr;
        }
        
        
        //로딩이 성공하면 cache 에 넣는다
        auto ret = this->textures.insert(std::make_pair(uri, new_tex));
        if (!ret.second)
        {
            TOD_LOG("warning", "duplicated texture hash [%s]\n", uri.c_str());
        }

        i = ret.first;
    }
    
    return i->second.get();
}

}
