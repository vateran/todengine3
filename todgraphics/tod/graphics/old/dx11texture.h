#pragma once
#include <D3D11.h>
#include "tod/graphics/texture.h"
namespace tod::graphics
{

class Dx11Texture : public Derive<Dx11Texture, Texture>
{
public:
    Dx11Texture()
        : d3d11texture(nullptr)
    {

    }

    bool create(int32 width, int32 height, Format format, void* data = nullptr)
    {
        return true;
    }

    void destroy()
    {
    }

    bool load(const String& uri) override
    {
        TOD_RETURN_TRACE(false);
    }

    void use(int32 sampler_index = 0) override
    {

    }

    void setHorizontalWrap(TextureWrap value) override
    {
        this->hWrap = value;
    }
    
    void setVerticalWrap(TextureWrap value) override
    {
        this->vWrap = value;
    }
    
    void setMinFilter(TextureFilter value) override
    {
        this->minFilter = value;
    }
    
    void setMagFilter(TextureFilter value) override
    {
        this->magFilter = value;
    }
    
    void setBorderColor(const ColorF& color) override
    {
        this->borderColor = color;
    }

protected:
    ID3D11Texture2D* d3d11texture;
};

}
