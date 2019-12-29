#pragma once
#include "tod/graphics/renderer.h"
#include "tod/graphics/d3dx11.h"
#include "tod/graphics/config.h"
#include <D3D11.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "tod/graphics/colorf.h"
namespace tod::graphics
{


class Dx11Shader
{
public:
    void setFileName(const String& file_name)
    {
        this->load_shader(file_name);
    }
    const String& getFileName()
    {
        return this->fileName;
    }

private:
    bool load_shader(const String& file_name)
    {
        if (!FileSystem::instance()->load(file_name,
            [this, file_name](FileSystem::Data& data)
        {
            ID3DBlob* shader = nullptr;
            ID3DBlob* error_buf = nullptr;

            UINT flag = 0;
            flag |= Config::instance()->isShaderDebug() ? D3DCOMPILE_DEBUG : 0;
            flag |= Config::instance()->isShaderOptimization() ? D3DCOMPILE_SKIP_OPTIMIZATION : 0;

            if (FAILED(D3DCompile(&data[0], data.size(), "",
                nullptr, nullptr, "main", "ps_5_0", flag, 0,
                &shader, &error_buf)))
            {
                TOD_THROW_EXCEPTION("VertexShader compile error[%s]:\n\n%s",
                    file_name, error_buf->GetBufferPointer());
                return false;
            }

            return true;
        }, FileSystem::LoadOption().string())) return false;

        return true;
    }

private:
    String fileName;
};


class Dx11Renderer : public Derive<Dx11Renderer, Renderer>
{
public:
    Dx11Renderer()
        : targetHWND(nullptr)
    {

    }
    virtual~Dx11Renderer()
    {
    }

    bool initialize(void* window_handle, int32 width, int32 height, bool windowed) override;
    void finalize() override;

    void resetDevice()
    {

    }

    bool render(Node* scene_root) override;

    Shader* createShader() override { return nullptr; }
    Mesh* createMesh(const String& name = S("")) override { return nullptr; }
    Texture* createTexture(const String& name = S("")) override { return nullptr; }
    RenderTarget* createRenderTarget(const String& name) override { return nullptr; }
    Camera* createCamera() override { return nullptr; }

private:
    HWND targetHWND;
    ID3D11Device* d3ddevice;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11DeviceContext *d3dcontext;
    IDXGISwapChain *swapChain;
    ID3D11RenderTargetView* backBufferRTV;
};

}
