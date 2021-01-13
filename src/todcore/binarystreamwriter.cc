#include "tod/precompiled.h"
#include "tod/binarystreamwriter.h"
namespace tod
{

//-----------------------------------------------------------------------------
BinaryStreamWriter::BinaryStreamWriter()
: buffer(nullptr)
, pos(0)
, bufferSize(0)
{
}


//-----------------------------------------------------------------------------
BinaryStreamWriter::~BinaryStreamWriter()
{
    TOD_SAFE_ARRAY_DELETE(this->buffer);
}


//-----------------------------------------------------------------------------
void BinaryStreamWriter::clear()
{
    TOD_SAFE_ARRAY_DELETE(this->buffer);
    this->pos = 0;
    this->bufferSize = 0;
}


//-----------------------------------------------------------------------------
uint32 BinaryStreamWriter::size() const
{
    return this->pos;
}


//-----------------------------------------------------------------------------
void BinaryStreamWriter::write(const String& value)
{
    uint32 len = static_cast<uint32>(value.size());
    this->write(&len, sizeof(len));
    this->write(value.data(), len);
}


//-----------------------------------------------------------------------------
void BinaryStreamWriter::write(const void* buf, uint32 size)
{
    if (this->capacity() <= this->pos + size)
    {
        this->reserve(this->bufferSize * 2 + size);
    }

    memcpy(this->buffer + this->pos, buf, size);
    this->pos += size;
}


//-----------------------------------------------------------------------------
void BinaryStreamWriter::reserve(uint32 size)
{
    if (this->bufferSize < size)
    {
        int8* new_buffer = new int8[size];
        if ((nullptr != this->buffer)
            && (0 < this->pos))
        {
            memcpy(new_buffer, this->buffer, this->pos);
            TOD_SAFE_ARRAY_DELETE(this->buffer);
        }
        this->buffer = new_buffer;
        this->bufferSize = size;
    }
}


//-----------------------------------------------------------------------------
uint32 BinaryStreamWriter::capacity() const
{
    return this->bufferSize;
}


//-----------------------------------------------------------------------------
uint32 BinaryStreamWriter::remain() const
{
    return this->bufferSize - this->pos;
}


//-----------------------------------------------------------------------------
int8* BinaryStreamWriter::ptr() const
{
    return this->buffer;
}

}