#pragma once
#include "tod/basetype.h"
namespace tod
{

class BinaryWriter
{
public:
    enum SeekOrigin
    {
          CURRENT
        , START
        , END
    };

public:
    BinaryWriter(int8* pointer, uint32 size)
    : pointer(pointer)
    , origin(pointer)
    , bufferSize(size)
    {
    }

    BinaryWriter(Buffer& buffer, uint32 size)
    : bufferSize(size)
    {
        buffer.resize(size);
        this->origin = this->pointer = buffer.data();
    }

    template <typename T>
    inline void write(const T& value)
    {
        TOD_ASSERT(
            ((this->pointer + sizeof(value)) - this->origin) <= bufferSize
            , "크기넘침");

        memcpy(this->pointer, &value, sizeof(value));
        this->pointer += sizeof(value);
    }

    inline void write(const String& value)
    {
        uint32 len = static_cast<uint32>(value.size());

        TOD_ASSERT(
            ((this->pointer + sizeof(len) + len) - this->origin) <= bufferSize
            , "크기넘침");
        
        memcpy(this->pointer, &len, sizeof(len));
        memcpy(this->pointer + sizeof(len), value.data(), len);
        this->pointer += sizeof(len) + len;
    }

    inline void reset()
    {
        this->pointer = this->origin;
    }

    inline void seek(uint32 pos, SeekOrigin seek_origin)
    {
        switch (seek_origin)
        {
        case SeekOrigin::START:
            this->pointer = this->origin + pos;
            break;
        case SeekOrigin::CURRENT:
            this->pointer += pos;
            break;
        case SeekOrigin::END:
            this->pointer = this->origin + this->bufferSize - pos;
            break;
        }
    }

    inline int8* ptr()
    {
        return this->pointer;
    }

    inline int8* originPtr()
    {
        return this->origin;
    }

    inline uint32 pos() const
    {
        return static_cast<uint32>(this->pointer - this->origin);
    }

    inline uint32 size() const
    {
        return this->bufferSize;
    }

    inline uint32 remain() const
    {
        return this->bufferSize - this->pos();
    }

private:
    int8* pointer;
    int8* origin;
    uint32 bufferSize;
};

}