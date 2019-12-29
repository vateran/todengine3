#include "tod/graphics/dx11renderinterface.h"
#include "tod/graphics/dx11constantbuffer.h"
#include "tod/graphics/dx11shader.h"
#include "tod/graphics/dx11texture.h"
#include "tod/graphics/dx11vertexbuffer.h"
#include "tod/graphics/dx11indexbuffer.h"
#include "tod/graphics/dx11viewport.h"
#include "tod/graphics/dx11inputlayout.h"
#include "tod/exception.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
DX11RenderInterface::DX11RenderInterface()
: d3ddevice(nullptr)
, featureLevel(D3D_FEATURE_LEVEL_11_0)
, d3dcontext(nullptr)
, dxgiDevice(nullptr)
, dxgiAdapter(nullptr)
, dxgiFactory(nullptr)
{
    memset(this->stockRasterizerState, 0, sizeof(this->stockRasterizerState));
}


//-----------------------------------------------------------------------------
DX11RenderInterface::~DX11RenderInterface()
{
    destroy();
}


//-----------------------------------------------------------------------------
bool DX11RenderInterface::create()
{
    uint32 createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    uint32 numFeatureLevels = ARRAYSIZE(featureLevels);

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    uint32 numDriverTypes = ARRAYSIZE(driverTypes);
    for (uint32 driverTypeIndex = 0
        ; driverTypeIndex < numDriverTypes
        ; ++driverTypeIndex)
    {
        auto hr = D3D11CreateDevice(
            nullptr,
            driverTypes[driverTypeIndex],
            nullptr,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &this->d3ddevice,
            &this->featureLevel,
            &this->d3dcontext);
        if (SUCCEEDED(hr))
        {
            break;
        }
    }

    if (FAILED(this->d3ddevice->QueryInterface(
        __uuidof(IDXGIDevice2)
        , (void**)&this->dxgiDevice)))
    {
        TOD_RETURN_TRACE(false);
    }

    if (FAILED(this->dxgiDevice->GetParent(
        __uuidof(IDXGIAdapter)
        , (void**)&this->dxgiAdapter)))
    {
        TOD_RETURN_TRACE(false);
    }

    if (FAILED(this->dxgiAdapter->GetParent(
        __uuidof(IDXGIFactory2)
        , (void**)&dxgiFactory)))
    {
        TOD_RETURN_TRACE(false);
    }

    if (false == this->create_stock_rasterizerstates())
    {
        TOD_RETURN_TRACE(false);
    }

    this->d3dcontext->RSSetState(this->stockRasterizerState[RS_FILLMODE_SOLID]);

    return true;
}


//-----------------------------------------------------------------------------
bool DX11RenderInterface::create_stock_rasterizerstates()
{
    {
        D3D11_RASTERIZER_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.CullMode = D3D11_CULL_BACK;
        desc.FillMode = D3D11_FILL_WIREFRAME;
        if (FAILED(this->d3ddevice->CreateRasterizerState(
            &desc, &this->stockRasterizerState[RS_FILLMODE_WIREFRAME])))
        {
            TOD_RETURN_TRACE(false);
        }

        desc.FillMode = D3D11_FILL_SOLID;
        if (FAILED(this->d3ddevice->CreateRasterizerState(
            &desc, &this->stockRasterizerState[RS_FILLMODE_SOLID])))
        {
            TOD_RETURN_TRACE(false);
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
void DX11RenderInterface::destroy()
{
    for (uint32 i = 0; i < RS_MAX; ++i)
    {
        TOD_SAFE_COM_RELEASE(this->stockRasterizerState[i]);
    }
    TOD_SAFE_COM_RELEASE(dxgiFactory);
    TOD_SAFE_COM_RELEASE(dxgiAdapter);
    TOD_SAFE_COM_RELEASE(dxgiDevice);
    TOD_SAFE_COM_RELEASE(this->d3dcontext);
    TOD_SAFE_COM_RELEASE(this->d3ddevice);
}


//-----------------------------------------------------------------------------
void DX11RenderInterface::beginScene()
{
}


//-----------------------------------------------------------------------------
void DX11RenderInterface::endScene()
{

}


//-----------------------------------------------------------------------------
Viewport* DX11RenderInterface::createViewport()
{
    return new DX11Viewport(this);
}


//-----------------------------------------------------------------------------
VertexBuffer* DX11RenderInterface::createVertexBuffer()
{
    return new DX11VertexBuffer(this);
}


//-----------------------------------------------------------------------------
IndexBuffer* DX11RenderInterface::createIndexBuffer()
{
    return new DX11IndexBuffer(this);
}


//-----------------------------------------------------------------------------
ConstantBuffer* DX11RenderInterface::createConstantBuffer(const String& name)
{
    auto name_hash = name.hash();
    auto i = this->constantBuffers.find(name_hash);
    if (this->constantBuffers.end() != i)
    {
        return i->second;
    }

    auto cb = new DX11ConstantBuffer(this);
    this->constantBuffers.emplace(name_hash, cb);
    return cb;
}


//-----------------------------------------------------------------------------
InputLayout* DX11RenderInterface::createInputLayout()
{
    return new DX11InputLayout(this);
}


//-----------------------------------------------------------------------------
Texture* DX11RenderInterface::createTexture()
{
    return new DX11Texture(this);
}


//-----------------------------------------------------------------------------
Shader* DX11RenderInterface::createShader()
{
    return new DX11Shader(this);
}


//-----------------------------------------------------------------------------
void DX11RenderInterface::drawPrimitive(uint32 count)
{
    this->d3dcontext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    this->d3dcontext->DrawIndexed(count, 0, 0);
}


//-----------------------------------------------------------------------------
void DX11RenderInterface::setWireframeMode(bool enable)
{
    if (true == enable)
    {
        this->d3dcontext->RSSetState(this->stockRasterizerState[RS_FILLMODE_WIREFRAME]);
    }
    else
    {
        this->d3dcontext->RSSetState(this->stockRasterizerState[RS_FILLMODE_SOLID]);
    }
}


//-----------------------------------------------------------------------------
ID3D11Device* DX11RenderInterface::getD3D11Device()
{
    return this->d3ddevice;
}


//-----------------------------------------------------------------------------
ID3D11DeviceContext* DX11RenderInterface::getD3D11DeviceContext()
{
    return this->d3dcontext;
}


//-----------------------------------------------------------------------------
IDXGIDevice2* DX11RenderInterface::getDXGIDevice()
{
    return this->dxgiDevice;
}


//-----------------------------------------------------------------------------
IDXGIAdapter* DX11RenderInterface::getDXGIAdapter()
{
    return this->dxgiAdapter;
}


//-----------------------------------------------------------------------------
IDXGIFactory2* DX11RenderInterface::getDXGIFactory()
{
    return this->dxgiFactory;
}


//-----------------------------------------------------------------------------
DXGI_FORMAT DX11RenderInterface::FormatToDXGI_FORMAT(Format format)
{
    //변환을 switch 에 추가해야함(추가후 MAX 값을 매칭시켜줄것)
    static_assert(static_cast<uint8>(Format::MAX) == 13);

    switch (format)
    {
    case Format::L8F:               return DXGI_FORMAT_R8_UNORM;
    case Format::R8G8B8A8:          return DXGI_FORMAT_R8G8B8A8_UINT;
    case Format::R8G8B8A8U:         return DXGI_FORMAT_R8G8B8A8_UINT;
    case Format::R8G8B8A8UN:        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Format::R8G8B8A8UN_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case Format::R16G16B16A16F:     return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case Format::R32F:              return DXGI_FORMAT_R32_FLOAT;
    case Format::R32G32F:           return DXGI_FORMAT_R32G32_FLOAT;
    case Format::R32G32B32F:        return DXGI_FORMAT_R32G32B32_FLOAT;
    case Format::R32G32B32A32F:     return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case Format::DEPTH16UN:         return DXGI_FORMAT_D16_UNORM;
    case Format::DEPTH32F:          return DXGI_FORMAT_D32_FLOAT;
    default:                        return DXGI_FORMAT_UNKNOWN;
    }
}


//-----------------------------------------------------------------------------
Format DX11RenderInterface::DXGI_FORMATToFormat(DXGI_FORMAT format)
{
    //변환을 switch 에 추가해야함(추가후 MAX 값을 매칭시켜줄것)
    static_assert(static_cast<uint8>(Format::MAX) == 13);

    switch (format)
    {
    case DXGI_FORMAT_R8_UNORM:              return Format::L8F;
    case DXGI_FORMAT_R8G8B8A8_UINT:         return Format::R8G8B8A8U;
    case DXGI_FORMAT_R8G8B8A8_UNORM:        return Format::R8G8B8A8UN;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return Format::R8G8B8A8UN_SRGB;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:    return Format::R16G16B16A16F;
    case DXGI_FORMAT_R32_FLOAT:             return Format::R32F;
    case DXGI_FORMAT_R32G32_FLOAT:          return Format::R32G32F;
    case DXGI_FORMAT_R32G32B32_FLOAT:       return Format::R32G32B32F;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:    return Format::R32G32B32A32F;
    case DXGI_FORMAT_D16_UNORM:             return Format::DEPTH16UN;
    case DXGI_FORMAT_D32_FLOAT:             return Format::DEPTH32F;
    default:                                return Format::MAX;
    }
}


}