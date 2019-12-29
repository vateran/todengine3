#pragma once
#include <D3D11.h>
#include <dxgi1_4.h>
#include "tod/graphics/viewport.h"
namespace tod::graphics
{

class DX11RenderInterface;
class DX11Viewport : public Viewport
{
public:
    DX11Viewport(DX11RenderInterface* render_interface);
    virtual~DX11Viewport();

    bool create(Window* window, uint32 multi_sample=1) override;
    void destroy() override;

    void use() override;
    void clear(const ColorF& clear_color) override;
    void swap() override;

    bool resize(uint32 width, uint32 height) override;

    void setMultiSample(uint32 multi_sample) override;
    uint32 getMultiSample() const override;
    uint32 getWidth() const override;
    uint32 getHeight() const override;

private:
    DX11RenderInterface* renderInterface;
    IDXGISwapChain1* swapChain;
    ID3D11RenderTargetView* backBufferRTV;
    ID3D11Texture2D* depthTexture;
    ID3D11DepthStencilState* depthState;
    ID3D11DepthStencilView* depthRTV;
    uint32 multiSample;
    Window* window;
    uint32 prevWindowWidth;
    uint32 prevWindowHeight;
};

}