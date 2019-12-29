#include "tod/graphics/dx11viewport.h"
#include "tod/window.h"
#include "tod/exception.h"
#include "tod/graphics/dx11renderinterface.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
DX11Viewport::DX11Viewport(DX11RenderInterface* render_interface)
: renderInterface(render_interface)
, swapChain(nullptr)
, backBufferRTV(nullptr)
, depthTexture(nullptr)
, depthState(nullptr)
, depthRTV(nullptr)
, multiSample(1)
, window(nullptr)
, prevWindowWidth(0)
, prevWindowHeight(0)
{
}


//-----------------------------------------------------------------------------
DX11Viewport::~DX11Viewport()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool DX11Viewport::create(Window* window, uint32 multi_sample)
{
    TOD_ASSERT(nullptr == this->swapChain, "이미 초기화됨");

    auto d3ddevice = this->renderInterface->getD3D11Device();
    auto d3dcontext = this->renderInterface->getD3D11DeviceContext();
    this->window = window;

    

    //swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapChainDesc.Width = window->getWidth();
    swapChainDesc.Height = window->getHeight();
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    swapChainDesc.Stereo = false;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = 0;
    swapChainDesc.SampleDesc.Count = multi_sample;
    swapChainDesc.SampleDesc.Quality = 0;    

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
    fullscreenDesc.RefreshRate.Numerator = 0;
    fullscreenDesc.RefreshRate.Numerator = 0;
    fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    fullscreenDesc.Windowed = true;

    if (FAILED(this->renderInterface->getDXGIFactory()->CreateSwapChainForHwnd(
        d3ddevice
        , static_cast<HWND>(window->getWindowHandle())
        , &swapChainDesc
        , &fullscreenDesc
        , nullptr
        , &this->swapChain)))
    {
        TOD_RETURN_TRACE(false);
    }

    if (false == this->resize(swapChainDesc.Width, swapChainDesc.Height))
    {
        TOD_RETURN_TRACE(false);
    }

    this->multiSample = multi_sample;

    return true;
}


//-----------------------------------------------------------------------------
void DX11Viewport::destroy()
{
    TOD_SAFE_COM_RELEASE(this->depthRTV);
    TOD_SAFE_COM_RELEASE(this->depthState);
    TOD_SAFE_COM_RELEASE(this->depthTexture);
    TOD_SAFE_COM_RELEASE(this->backBufferRTV);
    TOD_SAFE_COM_RELEASE(this->swapChain);
    this->window = nullptr;
    this->multiSample = 1;
}


//-----------------------------------------------------------------------------
void DX11Viewport::use()
{
    TOD_ASSERT(nullptr != this->renderInterface, "초기화 되지 않음");

    auto d3dcontext = this->renderInterface->getD3D11DeviceContext();
    
    {
        uint32 window_width, window_height;
        this->window->getSize(window_width, window_height);
        if ((window_width != this->prevWindowWidth)
         || (window_height != this->prevWindowHeight))
        {
            this->resize(window_width, window_height);
        }
    }

    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(this->window->getWidth());
    vp.Height = static_cast<FLOAT>(this->window->getHeight());
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    d3dcontext->RSSetViewports(1, &vp);

    d3dcontext->OMSetDepthStencilState(this->depthState, 1);

    d3dcontext->OMSetRenderTargets(
        1
        , &this->backBufferRTV
        , this->depthRTV);
}


//-----------------------------------------------------------------------------
void DX11Viewport::clear(const ColorF& clear_color)
{
    TOD_ASSERT(nullptr != this->renderInterface, "초기화 되지 않음");

    auto d3dcontext = this->renderInterface->getD3D11DeviceContext();

    d3dcontext->ClearRenderTargetView(this->backBufferRTV, clear_color);
    d3dcontext->ClearDepthStencilView(this->depthRTV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


//-----------------------------------------------------------------------------
void DX11Viewport::swap()
{
    TOD_ASSERT(nullptr != this->swapChain, "초기화 되지 않음");

    this->swapChain->Present(0, 0);
}


//-----------------------------------------------------------------------------
bool DX11Viewport::resize(uint32 width, uint32 height)
{
    TOD_ASSERT(nullptr != this->swapChain, "초기화 되지 않음");


    auto d3ddevice = this->renderInterface->getD3D11Device();
    auto d3dcontext = this->renderInterface->getD3D11DeviceContext();


    ID3D11RenderTargetView* null_views[] = { nullptr };
    d3dcontext->OMSetRenderTargets(ARRAYSIZE(null_views), null_views, nullptr);
    TOD_SAFE_COM_RELEASE(this->depthRTV);
    TOD_SAFE_COM_RELEASE(this->depthState);
    TOD_SAFE_COM_RELEASE(this->depthTexture);
    TOD_SAFE_COM_RELEASE(this->backBufferRTV);
    d3dcontext->Flush();


    HRESULT hr;
    if (FAILED(hr = this->swapChain->ResizeBuffers(
        2,
        width,
        height,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        0)))
    {
        if ((DXGI_ERROR_DEVICE_REMOVED == hr)
         || (DXGI_ERROR_DEVICE_RESET == hr))
        {
            return false;
        }

        TOD_RETURN_TRACE(false);
    }


    //Get BackBuffer RTV
    ID3D11Texture2D* backBuffer = nullptr;
    this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    d3ddevice->CreateRenderTargetView(backBuffer, nullptr, &this->backBufferRTV);
    backBuffer->Release();


    //Depth Buffer
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    memset(&depthBufferDesc, 0, sizeof(depthBufferDesc));
    depthBufferDesc.Width = window->getWidth();
    depthBufferDesc.Height = window->getHeight();
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = this->multiSample;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    if (FAILED(d3ddevice->CreateTexture2D(
        &depthBufferDesc
        , nullptr
        , &this->depthTexture)))
    {
        TOD_RETURN_TRACE(false);
    }


    D3D11_DEPTH_STENCIL_DESC depthDesc;
    memset(&depthDesc, 0, sizeof(depthDesc));
    depthDesc.DepthEnable = true;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDesc.StencilEnable = true;
    depthDesc.StencilReadMask = 0xFF;
    depthDesc.StencilWriteMask = 0xFF;
    depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    if (FAILED(d3ddevice->CreateDepthStencilState(
        &depthDesc
        , &this->depthState)))
    {
        TOD_RETURN_TRACE(false);
    }


    D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
    memset(&depthViewDesc, 0, sizeof(depthViewDesc));
    depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if (1 < this->multiSample)
    {
        depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    }
    else
    {
        depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    }
    depthViewDesc.Texture2D.MipSlice = 0;
    if (FAILED(d3ddevice->CreateDepthStencilView(
        this->depthTexture
        , &depthViewDesc
        , &this->depthRTV)))
    {
        TOD_RETURN_TRACE(false);
    }

    this->prevWindowWidth = width;
    this->prevWindowHeight = height;

    return true;
}


//-----------------------------------------------------------------------------
void DX11Viewport::setMultiSample(uint32 multi_sample)
{
    this->multiSample = multi_sample;
}


//-----------------------------------------------------------------------------
uint32 DX11Viewport::getMultiSample() const
{
    return this->multiSample;
}


//-----------------------------------------------------------------------------
uint32 DX11Viewport::getWidth() const
{
    TOD_ASSERT(nullptr != this->window, "초기화 되지 않음");

    return this->window->getWidth();
}


//-----------------------------------------------------------------------------
uint32 DX11Viewport::getHeight() const
{
    TOD_ASSERT(nullptr != this->window, "초기화 되지 않음");

    return this->window->getHeight();
}
    
}