#pragma once
#include "tod/graphics/renderer.h"
#include "tod/graphics/d3dx12.h"
#include <D3D12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
namespace tod::graphics
{


class Config : public SingletonDerive<Config, Node>
{
public:
    Config() :
        shaderDebug(false),
        shaderOptimization(false)
    {}

    void setShaderDebug(bool value) { this->shaderDebug = value; }
    bool isShaderDebug() { return this->shaderDebug; }
    void setShaderOptimization(bool value) { this->shaderDebug = value; }
    bool isShaderOptimization() { return this->shaderOptimization; }

    static void bindProperty()
    {
        BIND_PROPERTY(bool, "shader_debug", "Shader 디버깅 활성화", setShaderDebug, isShaderDebug, false, PropertyAttr::DEFAULT);
        BIND_PROPERTY(bool, "shader_optimization", "Shader 최적화 활성화", setShaderOptimization, isShaderOptimization, false, PropertyAttr::DEFAULT);
    }

private:
    bool shaderDebug;
    bool shaderOptimization;
};


class Dx12Shader
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

            D3D12_SHADER_BYTECODE shader_byte_code = {};
            shader_byte_code.BytecodeLength = shader->GetBufferSize();
            shader_byte_code.pShaderBytecode = shader->GetBufferPointer();

            return true;
        }, FileSystem::LoadOption().string())) return false;

        return true;
    }

private:
    String fileName;
};


class Dx12Renderer : public Derive<Dx12Renderer, Renderer>
{
public:
    virtual~Dx12Renderer()
    {
        this->finalize();
    }
    bool initialize(void* window_handle, int width, int height, bool windowed) override
    {
        this->targetHWND = static_cast<HWND>(window_handle);


        if (!this->init_d3d()) return false;


        //Command Queue 생성
        D3D12_COMMAND_QUEUE_DESC queue_desc;
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queue_desc.NodeMask = 0;

        if (FAILED(this->d3ddevice->CreateCommandQueue(
            &queue_desc, IID_PPV_ARGS(&this->commandQueue))))
        {
            TOD_THROW_EXCEPTION("CreateCommandQueue");
            return false;
        }

        if (!this->init_frame_buffer(width, height, windowed)) return false;

        if (!this->init_render_target()) return false;

        return true;

    }

    void mainloop() override
    {
    }

    struct Vertex
    {
        Vector3 pos;
    };

    bool render(Camera* camera, Node* scene_root) override
    {
        this->update_pipeline();

        ID3D12CommandList* command_list[] = { this->commandList };
        this->commandQueue->ExecuteCommandLists(
            _countof(command_list), command_list);

        if (FAILED(this->commandQueue->Signal(
            this->fence[this->curFrame],
            this->fenceValue[this->curFrame])))
        {
            return false;
        }

        if (FAILED(this->swapChain->Present(0, 0)))
        {
            return false;
        }

        return true;
    }

private:
    void update_pipeline()
    {
        this->wait_prev_frame();

        if (FAILED(this->commandAllocator[this->curFrame]->Reset()))
        {
            TOD_THROW_EXCEPTION(
                "this->commandAllocator[%d]->Reset()",
                this->curFrame);
            return;
        }

        if (FAILED(this->commandList->Reset(
            this->commandAllocator[this->curFrame], nullptr)))
        {
            TOD_THROW_EXCEPTION(
                "this->commandList->Reset(this->commandAllocator[%d])",
                this->curFrame);
            return;
        }


        //여기에 command list 에다가 recoding 을 한다



        this->commandList->ResourceBarrier(1,
            &CD3DX12_RESOURCE_BARRIER::Transition(
                this->renderTarget[this->curFrame],
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET));



        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(
            this->rtvDescHeap->GetCPUDescriptorHandleForHeapStart(),
            this->curFrame, this->rtvDescriptorSize);

        //OutputMege Stage에 RenderTarget을 설정 
        this->commandList->OMSetRenderTargets(1, &rtv_handle, false, nullptr);



        const float clear_color[] = { 0, 0, 1.0f, 1.0f };
        this->commandList->ClearRenderTargetView(
            rtv_handle, clear_color, 0, nullptr);




        this->commandList->ResourceBarrier(1,
            &CD3DX12_RESOURCE_BARRIER::Transition(
                this->renderTarget[this->curFrame],
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT));

        if (FAILED(this->commandList->Close()))
        {
            TOD_THROW_EXCEPTION("this->commandList->Close");
            return;
        }
    }

    bool init_d3d()
    {
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&this->dxFactory))))
        {
            TOD_THROW_EXCEPTION("CreateDXGIFactory1");
            return false;
        }

        IDXGIAdapter1* adapter = nullptr;
        int adapter_index = 0;
        bool adapter_fount = false;
        while (this->dxFactory->EnumAdapters1(adapter_index, &adapter)
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
                IID_PPV_ARGS(&this->d3ddevice))))
            {
                break;
            }

            ++adapter_index;
        }

        return this->d3ddevice != nullptr;
    }
    bool init_frame_buffer(int width, int height, bool windowed)
    {
        DXGI_MODE_DESC backbuffer_desc = {};
        backbuffer_desc.Width = width;
        backbuffer_desc.Height = height;
        backbuffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        DXGI_SAMPLE_DESC sample_desc = {};
        sample_desc.Count = 1;

        DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
        swap_chain_desc.BufferCount = NUM_FRAME_BUFFER;
        swap_chain_desc.BufferDesc = backbuffer_desc;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_chain_desc.OutputWindow = this->targetHWND;
        swap_chain_desc.SampleDesc = sample_desc;
        swap_chain_desc.Windowed = windowed;

        IDXGISwapChain* swap_chain = nullptr;
        HRESULT hr;
        if (FAILED(hr = this->dxFactory->CreateSwapChain(
            this->commandQueue, &swap_chain_desc, &swap_chain)))
        {
            TOD_THROW_EXCEPTION("CreateSwapChain");
            return false;
        }
        this->swapChain = static_cast<IDXGISwapChain3*>(swap_chain);
        this->curFrame = this->swapChain->GetCurrentBackBufferIndex();

        return true;
    }
    bool init_render_target()
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc_heap_desc = {};
        desc_heap_desc.NumDescriptors = NUM_FRAME_BUFFER;
        desc_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        if (FAILED(this->d3ddevice->CreateDescriptorHeap(
            &desc_heap_desc, IID_PPV_ARGS(&this->rtvDescHeap))))
        {
            TOD_THROW_EXCEPTION("CreateDescriptorHeap");
            return false;
        }

        this->rtvDescriptorSize = d3ddevice->
            GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(
            rtvDescHeap->GetCPUDescriptorHandleForHeapStart());

        for (int i = 0; i < NUM_FRAME_BUFFER; ++i)
        {
            if (FAILED(this->swapChain->GetBuffer(
                i, IID_PPV_ARGS(&this->renderTarget[i]))))
            {
                TOD_THROW_EXCEPTION("GetBuffer(%d)", i);
                return false;
            }

            //RenderTarget과 SwapChainBuffer를 bind
            this->d3ddevice->CreateRenderTargetView(
                this->renderTarget[i], nullptr, rtv_handle);

            rtv_handle.Offset(1, this->rtvDescriptorSize);
        }

        //Command Allocator 생성
        for (int i = 0; i < NUM_FRAME_BUFFER; ++i)
        {
            if (FAILED(this->d3ddevice->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(&this->commandAllocator[i]))))
            {
                TOD_THROW_EXCEPTION("CreateCommandAllocator(%d)", i);
                return false;
            }
        }

        //CommandList 생성(나중에 thread 렌더링 할때 여러개 만드는거 하자)
        //Allocator[i]에서 CommandList[i] 를 생성하는 듯?
        if (FAILED(this->d3ddevice->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT,
            this->commandAllocator[0], nullptr,
            IID_PPV_ARGS(&this->commandList))))
        {
            TOD_THROW_EXCEPTION("CreateCommandList");
            return false;
        }

        for (int i = 0; i < NUM_FRAME_BUFFER; ++i)
        {
            if (FAILED(this->d3ddevice->CreateFence(
                0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->fence[i]))))
            {
                TOD_THROW_EXCEPTION("CreateFence(%d)", i);
                return false;
            }
            this->fenceValue[i] = 0;
        }

        this->fenceEvent = CreateEvent(nullptr, false, false, nullptr);
        if (nullptr == this->fenceEvent)
        {
            TOD_THROW_EXCEPTION("this->fenceEvent = CreateEvent()");
            return false;
        }

        CD3DX12_ROOT_SIGNATURE_DESC signature_desc;
        signature_desc.Init(0, nullptr, 0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ID3DBlob* signature = nullptr;
        if (FAILED(D3D12SerializeRootSignature(
            &signature_desc, D3D_ROOT_SIGNATURE_VERSION_1,
            &signature, nullptr)))
        {
            TOD_THROW_EXCEPTION("D3D12SerializeRootSignature");
            return false;
        }

        if (FAILED(this->d3ddevice->CreateRootSignature(
            0, signature->GetBufferPointer(), signature->GetBufferSize(),
            IID_PPV_ARGS(&this->rootSignature))))
        {
            TOD_THROW_EXCEPTION("D3D12SerializeRootSignature");
            return false;
        }



        //VertexShader 생성(이 부분은 DX12Shader 로 빼야 함)
        {   
            const char* file_name = "file://VertexShader.hlsl";
            if (!FileSystem::instance()->load(file_name,
                [this, file_name](FileSystem::Data& data)
            {
                ID3DBlob* shader = nullptr;
                ID3DBlob* error_buf = nullptr;
                if (FAILED(D3DCompile(&data[0], data.size(), "",
                    nullptr, nullptr, "main", "vs_5_0",
                    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                    &shader, &error_buf)))
                {
                    TOD_THROW_EXCEPTION("VertexShader compile error[%s]:\n\n%s",
                        file_name, error_buf->GetBufferPointer());
                    return false;
                }

                D3D12_SHADER_BYTECODE shader_byte_code = {};
                shader_byte_code.BytecodeLength = shader->GetBufferSize();
                shader_byte_code.pShaderBytecode = shader->GetBufferPointer();

                return true;
            }, FileSystem::LoadOption().string())) return false;

            
        }


        //PixelShader 생성(이 부분은 DX12Shader 로 빼야 함)
        {
            const char* file_name = "file://PixelShader.hlsl";
            if (!FileSystem::instance()->load(file_name,
                [this, file_name](FileSystem::Data& data)
            {
                ID3DBlob* shader = nullptr;
                ID3DBlob* error_buf = nullptr;
                if (FAILED(D3DCompile(&data[0], data.size(), "",
                    nullptr, nullptr, "main", "ps_5_0",
                    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                    &shader, &error_buf)))
                {
                    TOD_THROW_EXCEPTION("VertexShader compile error[%s]:\n\n%s",
                        file_name, error_buf->GetBufferPointer());
                    return false;
                }

                D3D12_SHADER_BYTECODE shader_byte_code = {};
                shader_byte_code.BytecodeLength = shader->GetBufferSize();
                shader_byte_code.pShaderBytecode = shader->GetBufferPointer();

                return true;
            }, FileSystem::LoadOption().string())) return false;
        }



        this->commandList->Close();

        return true;
    }

    void wait_prev_frame()
    {
        this->curFrame = this->swapChain->GetCurrentBackBufferIndex();

        auto cur_fence = this->fence[this->curFrame];
        auto cur_fence_value = this->fenceValue[this->curFrame];
        if (cur_fence->GetCompletedValue() < cur_fence_value)
        {
            if (FAILED(cur_fence->SetEventOnCompletion(
                cur_fence_value, this->fenceEvent)))
            {
                TOD_THROW_EXCEPTION("cur_fence->SetEventOnCompletion");
                return;
            }

            WaitForSingleObject(this->fenceEvent, INFINITE);
        }

        ++this->fenceValue[this->curFrame];
    }

    void finalize()
    {
        //모든 background frame 작업이 끝나기를 기다린다
        for (int i = 0; i < NUM_FRAME_BUFFER; ++i)
        {
            this->curFrame = i;
            this->wait_prev_frame();
        }

        //fullscreen 화면을 끝낸다
        BOOL fs = false;
        if (this->swapChain->GetFullscreenState(&fs, nullptr))
            this->swapChain->SetFullscreenState(false, nullptr);

        for (int i = 0; i < NUM_FRAME_BUFFER; ++i)
        {
            SAFE_COM_RELEASE(this->fence[i]);
            SAFE_COM_RELEASE(this->commandAllocator[i]);
            SAFE_COM_RELEASE(this->renderTarget[i]);
        };

        if (this->fenceEvent)
        {
            CloseHandle(this->fenceEvent);
            this->fenceEvent = INVALID_HANDLE_VALUE;
        }
        SAFE_COM_RELEASE(this->rtvDescHeap);
        SAFE_COM_RELEASE(this->commandList);
        SAFE_COM_RELEASE(this->swapChain);
        SAFE_COM_RELEASE(this->commandQueue);
        SAFE_COM_RELEASE(this->d3ddevice);
        SAFE_COM_RELEASE(this->dxFactory);
    }

private:
    static const int NUM_FRAME_BUFFER = 3;

    HWND targetHWND;
    IDXGIFactory4* dxFactory;
    ID3D12Device* d3ddevice;
    ID3D12CommandQueue* commandQueue;
    IDXGISwapChain3* swapChain;
    ID3D12Resource* renderTarget[NUM_FRAME_BUFFER];
    ID3D12DescriptorHeap* rtvDescHeap;
    ID3D12CommandAllocator* commandAllocator[NUM_FRAME_BUFFER];
    ID3D12GraphicsCommandList* commandList;
    ID3D12Fence* fence[NUM_FRAME_BUFFER];
    ID3D12RootSignature* rootSignature;
    HANDLE fenceEvent;
    int rtvDescriptorSize;
    int curFrame;
    uint64 fenceValue[NUM_FRAME_BUFFER];

};

}