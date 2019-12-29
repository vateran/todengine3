#pragma once
#include <D3D11.h>
#include "tod/graphics/texture.h"
namespace tod::graphics
{

class DX11RenderInterface;
class DX11Texture : public Texture
{
public:
    DX11Texture(DX11RenderInterface* render_interface);
    virtual~DX11Texture();

    bool create(int32 width, int32 height, Format format, void* data = nullptr) override;
    void destroy() override;

    bool load(const String& uri, uint8 mipmap_level = 0) override;
    void use(int32 sampler_index = 0) override;

    void setHorizontalWrap(TextureWrap value) override;
    void setVerticalWrap(TextureWrap value) override;
    void setMinFilter(TextureFilter value) override;
    void setMagFilter(TextureFilter value) override;
    void setBorderColor(const ColorF& color) override;

    ID3D11ShaderResourceView* getSRV();

private:
    DX11RenderInterface* renderInterface;
    ID3D11Resource* d3dtexture2D;
    ID3D11ShaderResourceView* d3dtextureSRV;
};
    
}