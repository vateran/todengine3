#pragma once
#include <D3D11.h>
#include "tod/basetype.h"
#include "tod/graphics/vertexbuffer.h"
namespace tod::graphics
{

class DX11RenderInterface;
class DX11VertexBuffer : public VertexBuffer
{
public:
    DX11VertexBuffer(DX11RenderInterface* render_interface);
    virtual~DX11VertexBuffer();

    bool create(uint32 vertex_count, int32 stride) override;
    bool create(uint32 vertex_count, const void* vertex_data, int32 stride) override;
    void destroy() override;

    bool lock(int8** buffer_ptr) override;
    void unlock() override;
    void use(uint32 start_offset = 0) override;
    uint32 size() const override;

private:
    DX11RenderInterface* renderInterface;
    ID3D11Buffer* buffer;
    uint32 vertexCount;
    uint32 stride;
};
    
}