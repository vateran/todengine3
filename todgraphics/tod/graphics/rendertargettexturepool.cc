#include "tod/graphics/rendertargettexturepool.h"
#include "tod/graphics/renderer.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
RenderTargetTexturePool::RenderTargetTexturePool():
renderer("/sys/renderer")
{
}


//-----------------------------------------------------------------------------
Texture* RenderTargetTexturePool::getTexture
(int width, int height, PixelFormat format)
{
    auto key = std::make_tuple(width, height, format);
    
    //찾아보고
    auto i = this->rtTextures.find(key);
    
    //없으면 만들어서 줌
    if (this->rtTextures.end() == i)
    {
        auto texture = this->renderer->createTexture();
        if (nullptr == texture) TOD_RETURN_TRACE(nullptr);
        
        texture->create(width, height, format);
        texture->setMinFilter(TextureFilter::NEAREST);
        texture->setMagFilter(TextureFilter::NEAREST);
        texture->setHorizontalWrap(TextureWrap::CLAMP_TO_EDGE);
        texture->setVerticalWrap(TextureWrap::CLAMP_TO_EDGE);
        
        return texture;
    }
    
    return i->second;
}


//-----------------------------------------------------------------------------
void RenderTargetTexturePool::release(Texture* texture)
{
    auto key = std::make_tuple(
        texture->getWidth(),
        texture->getHeight(),
        texture->getFormat());

    this->rtTextures.insert(std::make_pair(key, texture));
}

}