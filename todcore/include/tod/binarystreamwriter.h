#pragma once
namespace tod
{

class BinaryStreamWriter
{
public:
    BinaryStreamWriter();
    ~BinaryStreamWriter();

    void clear();
    uint32 size() const;

    template <typename TYPE>
    void write(const TYPE& value)
    {
        this->write(&value, sizeof(value));
    }

    void write(const String& value);
    void write(const void* buf, uint32 size);

    void reserve(uint32 size);
    uint32 capacity() const;
    uint32 remain() const;
    int8* ptr() const;

private:
    int8* buffer;
    uint32 pos;
    uint32 bufferSize;
};

}