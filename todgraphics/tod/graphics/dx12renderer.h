#pragma once
#include "tod/graphics/renderer.h"
#include <D3D12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
namespace tod::graphics
{

class Dx12Renderer : public Derive<Dx12Renderer, Renderer>
{
public:
    bool initialize(void* window_handle) override
    {
        if (!this->init_d3d()) return false;

        return true;

    }

    void mainloop() override
    {
    }

    bool render(Camera* camera, Node* scene_root) override
    {
        return true;
    }

private:
    bool init_d3d()
    {
        IDXGIFactory4* gi_factory = nullptr;
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&gi_factory))))
        {
            TOD_THROW_EXCEPTION("CreateDXGIFactory1");
            return false;
        }

        IDXGIAdapter1* adapter = nullptr;
        int adapter_index = 0;
        bool adapter_fount = false;
        while (gi_factory->EnumAdapters1(adapter_index, &adapter)               
               != DXGI_ERROR_NOT_FOUND)
        {
            //Software adapter 는 건너뜀
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                ++adapter_index;
                continue;
            }

            //D3D12 이상 장치만 생성
            if (SUCCEEDED(D3D12CreateDevice(
                adapter, D3D_FEATURE_LEVEL_11_0,
                __uuidof(ID3D12Device),
                reinterpret_cast<void**>(&this->d3ddevice))))
            {
                break;
            }

            ++adapter_index;
        }

        gi_factory->Release();

        return this->d3ddevice != nullptr;
    }

private:
    ID3D12Device* d3ddevice;

};

}