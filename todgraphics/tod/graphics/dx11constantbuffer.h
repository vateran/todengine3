#pragma once
#include <D3D11.h>
#include <vector>
#include "tod/graphics/constantbuffer.h"
namespace tod::graphics
{

class DX11RenderInterface;
class DX11ConstantBuffer : public ConstantBuffer
{
public:
    DX11ConstantBuffer(DX11RenderInterface* render_interface);
    virtual~DX11ConstantBuffer();

    bool create(uint32 size) override;
    void destroy() override;

    bool isValid() override;

    ID3D11Buffer* getD3D11Buffer();

    bool unlock() override;

protected:
    void* getDataPtr() override;

private:
    bool lock_cbuffer(uint8** buffer_ptr);
    void unlock_cbuffer();

protected:
    DX11RenderInterface* renderInterface;
    ID3D11Buffer* buffer;
    std::vector<uint8> data;
};

}