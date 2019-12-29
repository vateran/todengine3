#pragma once
#include <D3D11.h>
#include "tod/basetype.h"
#include "tod/graphics/indexbuffer.h"
namespace tod::graphics
{

class DX11RenderInterface;
class DX11IndexBuffer : public IndexBuffer
{
public:
    DX11IndexBuffer(DX11RenderInterface* render_interface);
    virtual~DX11IndexBuffer();

    bool create(uint32 index_count, int32 stride) override;
    bool create(uint32 index_count, const void* index_data, int32 stride) override;
    void destroy() override;

    bool lock(int8** buffer_ptr) override;
    void unlock() override;

    void use(uint32 start_offset = 0) override;

    uint32 getIndexCount() override;
    uint32 size() const override;

private:
    void determin_format(int32 stride);

private:
    DX11RenderInterface* renderInterface;
    ID3D11Buffer* buffer;
    uint32 indexCount;
    uint32 bufferSize;
    DXGI_FORMAT bufferFormat;
};

}