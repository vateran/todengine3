#include "tod/graphics/dx11indexbuffer.h"
#include "tod/exception.h"
#include "tod/graphics/dx11renderinterface.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
DX11IndexBuffer::DX11IndexBuffer(DX11RenderInterface* render_interface)
: renderInterface(render_interface)
, buffer(nullptr)
, indexCount(0)
, bufferSize(0)
, bufferFormat(DXGI_FORMAT_UNKNOWN)
{

}


//-----------------------------------------------------------------------------
DX11IndexBuffer::~DX11IndexBuffer()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool DX11IndexBuffer::create(uint32 index_count, int32 stride)
{
    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.ByteWidth = static_cast<UINT>(index_count * stride);
    desc.StructureByteStride = 0;

    if (FAILED(this->renderInterface->getD3D11Device()->CreateBuffer(&desc, nullptr, &this->buffer)))
    {
        TOD_RETURN_TRACE(false);
    }

    this->indexCount = index_count;

    this->determin_format(stride);

    return true;
}


//-----------------------------------------------------------------------------
bool DX11IndexBuffer::create(uint32 index_count, const void* index_data, int32 stride)
{
    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = static_cast<UINT>(index_count * stride);
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = index_data;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    if (FAILED(this->renderInterface->getD3D11Device()->CreateBuffer(&desc, &data, &this->buffer)))
    {
        TOD_RETURN_TRACE(false);
    }

    this->indexCount = index_count;

    this->determin_format(stride);

    return true;
}


//-----------------------------------------------------------------------------
void DX11IndexBuffer::destroy()
{
    TOD_SAFE_COM_RELEASE(this->buffer);
    this->indexCount = 0;
    this->bufferFormat = DXGI_FORMAT_UNKNOWN;
}


//-----------------------------------------------------------------------------
bool DX11IndexBuffer::lock(int8** buffer_ptr)
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

    *buffer_ptr = static_cast<int8*>(resource.pData);

    return true;
}


//-----------------------------------------------------------------------------
void DX11IndexBuffer::unlock()
{
    this->renderInterface->getD3D11DeviceContext()->Unmap(this->buffer, 0);
}


//-----------------------------------------------------------------------------
void DX11IndexBuffer::use(uint32 start_offset)
{
    auto dx11context = renderInterface->getD3D11DeviceContext();
    dx11context->IASetIndexBuffer(this->buffer, this->bufferFormat, start_offset);
}


//-----------------------------------------------------------------------------
uint32 DX11IndexBuffer::getIndexCount()
{
    return this->indexCount;
}


//-----------------------------------------------------------------------------
uint32 DX11IndexBuffer::size() const
{
    return this->bufferSize;
}


//-----------------------------------------------------------------------------
void DX11IndexBuffer::determin_format(int32 stride)
{
    switch (stride)
    {
    case 2:
        this->bufferFormat = DXGI_FORMAT_R16_UINT;
        this->bufferSize = 2 * this->indexCount;
        break;
    case 4:
        this->bufferFormat = DXGI_FORMAT_R32_UINT;
        this->bufferSize = 4 * this->indexCount;
        break;
    }
}

}