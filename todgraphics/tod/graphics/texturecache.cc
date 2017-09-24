#include "tod/graphics/texturecache.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/renderer.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
TextureCache::TextureCache():
renderer("/sys/renderer")
{
    {
        auto empty_diffuse = this->renderer->createTexture();
        uint32 data = 0x050505;
        empty_diffuse->create(1, 1, PixelFormat::R8G8B8, &data);
        empty_diffuse->setMinFilter(TextureFilter::LINEAR);
        empty_diffuse->setMagFilter(TextureFilter::LINEAR);
        empty_diffuse->setHorizontalWrap(TextureWrap::REPEAT);
        empty_diffuse->setVerticalWrap(TextureWrap::REPEAT);
        this->textures.insert(std::make_pair(
            String("empty_diffuse").hash(), empty_diffuse));
    }
    
    {
        auto empty_normal = this->renderer->createTexture();
        uint32 data = 0xff0000;
        empty_normal->create(1, 1, PixelFormat::R8G8B8, &data);
        empty_normal->setMinFilter(TextureFilter::LINEAR);
        empty_normal->setMagFilter(TextureFilter::LINEAR);
        empty_normal->setHorizontalWrap(TextureWrap::REPEAT);
        empty_normal->setVerticalWrap(TextureWrap::REPEAT);
        this->textures.insert(std::make_pair(
            String("empty_normal").hash(), empty_normal));
    }
    
    {
        auto empty_specular = this->renderer->createTexture();
        uint32 data = 0x111111;
        empty_specular->create(1, 1, PixelFormat::R8G8B8, &data);
        empty_specular->setMinFilter(TextureFilter::LINEAR);
        empty_specular->setMagFilter(TextureFilter::LINEAR);
        empty_specular->setHorizontalWrap(TextureWrap::REPEAT);
        empty_specular->setVerticalWrap(TextureWrap::REPEAT);
        this->textures.insert(std::make_pair(
            String("empty_specular").hash(), empty_specular));
    }
}


//-----------------------------------------------------------------------------
Texture* TextureCache::getTexture(const String& uri)
{
    auto uri_hash = uri.hash();
    auto i = this->textures.find(uri_hash);
    if (this->textures.end() == i)
    {
        //새로운 Texture를 만들고 로딩해본다
        auto new_tex = this->renderer->createTexture();
        if (!new_tex->load(uri))
        {
            new_tex->release();
            return nullptr;
        }
        
        
        //로딩이 성공하면 cache 에 넣는다
        auto ret = this->textures.insert(std::make_pair(uri_hash, new_tex));
        if (!ret.second)
            TOD_LOG("warning", "duplicated texture hash [%s]\n", uri.c_str());
        i = ret.first;
    }
    
    return i->second.get();
}

}
