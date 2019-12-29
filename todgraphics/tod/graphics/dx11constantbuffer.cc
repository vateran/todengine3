#include "tod/graphics/dx11constantbuffer.h"
#include "tod/graphics/dx11renderinterface.h"
#include "tod/exception.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
DX11ConstantBuffer::DX11ConstantBuffer(DX11RenderInterface* render_interface)
: renderInterface(render_interface)
, buffer(nullptr)
{

}


//-----------------------------------------------------------------------------
DX11ConstantBuffer::~DX11ConstantBuffer()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool DX11ConstantBuffer::create(uint32 size)
{
    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.ByteWidth = size;
    desc.StructureByteStride = 0;

    if (FAILED(this->renderInterface->getD3D11Device()->CreateBuffer(
        &desc
        , nullptr
        , &this->buffer)))
    {
        TOD_RETURN_TRACE(false);
    }

    this->data.resize(size);

    return true;
}


//-----------------------------------------------------------------------------
void DX11ConstantBuffer::destroy()
{
    TOD_SAFE_COM_RELEASE(this->buffer);
}


//-----------------------------------------------------------------------------
bool DX11ConstantBuffer::isValid()
{
    return this->buffer != nullptr;
}


//-----------------------------------------------------------------------------
ID3D11Buffer* DX11ConstantBuffer::getD3D11Buffer()
{
    return this->buffer;
}


//-----------------------------------------------------------------------------
bool DX11ConstantBuffer::unlock()
{
    uint8* ptr = nullptr;
    if (false == this->lock_cbuffer(&ptr)) TOD_RETURN_TRACE(false);
    memcpy(ptr, &this->data[0], this->data.size());
    this->unlock_cbuffer();

    return true;
}


//-----------------------------------------------------------------------------
void* DX11ConstantBuffer::getDataPtr()
{
    return &this->data[0];
}


//-----------------------------------------------------------------------------
bool DX11ConstantBuffer::lock_cbuffer(uint8** buffer_ptr)
{
    D3D11_MAPPED_SUBRESOURCE resource;
    if (FAILED(this->renderInterface->getD3D11DeviceContext()->Map(
        this->buffer
        , 0
        , D3D11_MAP_WRITE_DISCARD
        , 0
        , &resource)))
    {
        TOD_RETURN_TRACE(false);
    }

    *buffer_ptr = static_cast<uint8*>(resource.pData);

    return true;
}


//-----------------------------------------------------------------------------
void DX11ConstantBuffer::unlock_cbuffer()
{
    this->renderInterface->getD3D11DeviceContext()->Unmap(this->buffer, 0);
};

}