#pragma once
#include <vector>
namespace tod::graphics
{

class IndexBuffer
{
public:
    virtual~IndexBuffer() {}

    template <typename INDEX_TYPE>
    bool create(const std::vector<INDEX_TYPE>& data);

    virtual bool create(uint32 index_count, int32 stride) = 0;
    virtual bool create(uint32 index_count, const void* index_data, int32 stride) = 0;
    virtual void destroy() = 0;
    virtual bool lock(int8** out_buffer_ptr_must_casting) = 0;
    virtual void unlock() = 0;
    virtual void use(uint32 start_offset = 0) = 0;
    virtual uint32 getIndexCount() = 0;
    virtual uint32 size() const = 0;

protected:
};

}