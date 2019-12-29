#pragma once
#include <vector>
namespace tod::graphics
{

class VertexBuffer
{
public:
    virtual~VertexBuffer() {}

    template <typename VERTEX_TYPE>
    bool create(const std::vector<VERTEX_TYPE>& data);

    virtual bool create(uint32 vertex_count, int32 stride) = 0;
    virtual bool create(uint32 vertex_count, const void* vertex_data, int32 stride) = 0;
    virtual void destroy() = 0;
    virtual bool lock(int8** buffer_ptr) = 0;
    virtual void unlock() = 0;
    virtual void use(uint32 start_offset = 0) = 0;
    virtual uint32 size() const = 0;
};


template <typename VERTEX_TYPE>
bool VertexBuffer::create(const std::vector<VERTEX_TYPE>& data)
{
    return create(
        static_cast<uint32>(data.size())
        , data.data()
        , sizeof(VERTEX_TYPE));
}

}