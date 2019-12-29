#pragma once
#include <D3D11.h>
#include <dxgi1_4.h>
#include "tod/string.h"
#include "tod/graphics/graphicsdef.h"
#include "tod/graphics/renderinterface.h"
namespace tod::graphics
{

class DX11ConstantBuffer;
class DX11RenderInterface : public RenderInterface
{
public:
    enum
    {
        CMD_DrawPrimitive,

        CMD_MAX,
    };

public:
    DX11RenderInterface();
    virtual~DX11RenderInterface();

    bool create() override;
    void destroy() override;

    void beginScene() override;
    void endScene() override;

    Viewport* createViewport() override;
    VertexBuffer* createVertexBuffer() override;
    IndexBuffer* createIndexBuffer() override;
    ConstantBuffer* createConstantBuffer(const String& name) override;
    InputLayout* createInputLayout() override;
    Texture* createTexture() override;
    Shader* createShader() override;

    void drawPrimitive(uint32 count) override;
        
    void setWireframeMode(bool enable) override;

    ID3D11Device* getD3D11Device();
    ID3D11DeviceContext* getD3D11DeviceContext();
    IDXGIDevice2* getDXGIDevice();
    IDXGIAdapter* getDXGIAdapter();
    IDXGIFactory2* getDXGIFactory();

    static DXGI_FORMAT FormatToDXGI_FORMAT(Format format);
    static Format DXGI_FORMATToFormat(DXGI_FORMAT format);

private:
    typedef void (DX11RenderInterface::* CommandFunc)();
    static CommandFunc s_commandFuncs[CMD_MAX];

private:
    enum
    {
        RS_FILLMODE_WIREFRAME,
        RS_FILLMODE_SOLID,

        RS_MAX,
    };

private:
    bool create_stock_rasterizerstates();

private:
    ID3D11Device* d3ddevice;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11DeviceContext* d3dcontext;
    IDXGIDevice2* dxgiDevice;
    IDXGIAdapter* dxgiAdapter;
    IDXGIFactory2* dxgiFactory;
    ID3D11RasterizerState* stockRasterizerState[RS_MAX];

    typedef std::unordered_map<int32, DX11ConstantBuffer*> ConstantBuffers;
    ConstantBuffers constantBuffers;
};

}