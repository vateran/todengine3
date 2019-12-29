#include "tod/graphics/dx11vertexbuffer.h"
#include "tod/graphics/dx11renderinterface.h"
#include "tod/exception.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
DX11VertexBuffer::DX11VertexBuffer(DX11RenderInterface* render_interface)
: renderInterface(render_interface)
, buffer(nullptr)
, vertexCount(0)
, stride(0)
{

}


//-----------------------------------------------------------------------------
DX11VertexBuffer::~DX11VertexBuffer()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool DX11VertexBuffer::create(uint32 vertex_count, int32 stride)
{
    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.ByteWidth = static_cast<UINT>(vertex_count * stride);
    desc.StructureByteStride = 0;

    if (FAILED(this->renderInterface->
        getD3D11Device()->CreateBuffer(&desc, nullptr, &this->buffer)))
    {
        TOD_RETURN_TRACE(false);
    }

    this->vertexCount = vertex_count;
    this->stride = stride;

    return true;
}


//-----------------------------------------------------------------------------
bool DX11VertexBuffer::create(uint32 vertex_count, const void* vertex_data, int32 stride)
{
    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = static_cast<UINT>(vertex_count * stride);
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = vertex_data;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    if (FAILED(this->renderInterface->getD3D11Device()->CreateBuffer(
        &desc
        , &data
        , &this->buffer)))
    {
        TOD_RETURN_TRACE(false);
    }

    this->vertexCount = vertex_count;
    this->stride = stride;

    return true;
}


//-----------------------------------------------------------------------------
void DX11VertexBuffer::destroy()
{
    TOD_SAFE_COM_RELEASE(this->buffer);
    this->vertexCount = 0;
    this->stride = 0;
}


//-----------------------------------------------------------------------------
bool DX11VertexBuffer::lock(int8** buffer_ptr)
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
void DX11VertexBuffer::unlock()
{
    this->renderInterface->getD3D11DeviceContext()->Unmap(this->buffer, 0);
}


//-----------------------------------------------------------------------------
void DX11VertexBuffer::use(uint32 start_offset)
{
    auto dx11context = renderInterface->getD3D11DeviceContext();
    dx11context->
        IASetVertexBuffers(0, 1, &this->buffer, &this->stride, &start_offset);
}


//-----------------------------------------------------------------------------
uint32 DX11VertexBuffer::size() const
{
    return this->vertexCount * this->stride;
}

}