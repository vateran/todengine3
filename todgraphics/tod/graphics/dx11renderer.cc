#include "tod/graphics/dx11renderer.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

namespace tod::graphics
{

//----------------------------------------------------------------------------
bool Dx11Renderer::initialize
(void* window_handle, int32 width, int32 height, bool windowed)
{
    this->targetHWND = static_cast<HWND>(window_handle);
    this->featureLevel = D3D_FEATURE_LEVEL_11_0;


    //swap chain
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    memset(&swap_chain_desc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferDesc.Width = width;
    swap_chain_desc.BufferDesc.Height = height;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    swap_chain_desc.OutputWindow = this->targetHWND;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.Windowed = windowed;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    
    UINT createDeviceFlags = 0;
    #ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    uint32 numDriverTypes = ARRAYSIZE(driverTypes);
    for (uint32 driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
    {
        auto hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            driverTypes[driverTypeIndex],
            nullptr,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &this->swapChain,
            &this->d3ddevice,
            &this->featureLevel,
            &this->d3dcontext);
        if (SUCCEEDED(hr))
        {
            break;
        }
    }


    //Create Context
    HWND hwnd = (HWND)window_handle;


    //Get BackBuffer RTV
    ID3D11Texture2D* backBuffer = nullptr;
    this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    this->d3ddevice->CreateRenderTargetView(backBuffer, nullptr, &this->backBufferRTV);
    backBuffer->Release();

    
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    this->d3dcontext->RSSetViewports(1, &vp);


    //if (!BaseType::initialize(window_handle, width, height, windowed))
      //  TOD_RETURN_TRACE(false);


    return true;
}


//----------------------------------------------------------------------------
void Dx11Renderer::finalize()
{   
    TOD_SAFE_COM_RELEASE(this->backBufferRTV);
    TOD_SAFE_COM_RELEASE(this->swapChain);
    TOD_SAFE_COM_RELEASE(this->d3dcontext);
    TOD_SAFE_COM_RELEASE(this->d3ddevice);

    BaseType::finalize();
}


//----------------------------------------------------------------------------
bool Dx11Renderer::render(Node* scene_root)
{
    d3dcontext->ClearRenderTargetView(this->backBufferRTV, ColorF(0, 0, 1, 0));

    this->swapChain->Present(0, 0);

    return true;
}

}
