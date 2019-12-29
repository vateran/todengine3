#include "tod/precompiled.h"
#include "tod/file.h"
namespace tod
{

//-----------------------------------------------------------------------------
const char* File::OpenMode::str() const
{
    static String::value_type*
        open_letter[] = { S("w"), S("r"), S("a"), S("b"), S("t") };
    this->modeStr.clear();
    for (int32 i = 0; i < MAX; ++i)
    {
        if (this->flags[i])
        {
            this->modeStr.append(open_letter[i]);
        }
    }

    return this->modeStr.c_str();
}


//-----------------------------------------------------------------------------
File::File() : fp(nullptr)
{
}


//-----------------------------------------------------------------------------
File::File(const char* fname, const OpenMode& open_mode) : fp(nullptr)
{
    this->open(fname, open_mode);
}


//-----------------------------------------------------------------------------
File::File(const String& fname, const OpenMode& open_mode) : fp(nullptr)
{
    this->open(fname, open_mode);
}


//-----------------------------------------------------------------------------
File::~File()
{
    this->close();
}


//-----------------------------------------------------------------------------
bool File::open(const char* fname, const OpenMode& open_mode)
{
    openMode = open_mode;

#if defined(TOD_PLATFORM_WINDOWS)
    fopen_s(&this->fp, fname, this->openMode.str());
#else
#endif

    return this->valid();
}


//-----------------------------------------------------------------------------
bool File::open(const String& fname, const OpenMode& open_mode)
{
    return this->open(fname.c_str(), open_mode);
}


//-----------------------------------------------------------------------------
bool File::valid() const
{
    return (nullptr != this->fp);
}


//-----------------------------------------------------------------------------
void File::close()
{
    if (nullptr != this->fp)
    {
        fclose(this->fp);
        this->fp = nullptr;
    }
}


//-----------------------------------------------------------------------------
uint32 File::write(const void* data, uint32 size)
{
    TOD_ASSERT(nullptr != this->fp, "open 해야함");
    TOD_ASSERT(true == this->openMode.isWrite(), "write로 오픈안함");
    fwrite(data, size, 1, this->fp);
    return size;
}


//-----------------------------------------------------------------------------
uint32 File::read(void* data_out, uint32 size)
{
    TOD_ASSERT(nullptr != this->fp, "open 해야함");
    TOD_ASSERT(true == this->openMode.isRead(), "read로 오픈안함");
    fread(data_out, size, 1, this->fp);
    return size;
}


//-----------------------------------------------------------------------------
void File::seek(int32 offset, SeekOrigin seek_origin)
{
    TOD_ASSERT(nullptr != this->fp, "open 해야함");

    int32 so;
    switch (seek_origin)
    {
    case SeekOrigin::START:     so = SEEK_SET; break;
    case SeekOrigin::CURRENT:   so = SEEK_CUR; break;
    case SeekOrigin::END:       so = SEEK_END; break;
    default:                    so = SEEK_CUR; break;
    }

    fseek(this->fp, offset, so);
}


//-----------------------------------------------------------------------------
uint32 File::current()
{
    TOD_ASSERT(nullptr != this->fp, "open 해야함");
    return ftell(this->fp);
}


//-----------------------------------------------------------------------------
uint32 File::write(const String& value)
{
    uint32 size = 0;
    size += this->write<uint32>(static_cast<uint32>(value.size()));
    if (false == value.empty())
    {
        size += this->write(
              reinterpret_cast<const int8*>(value.c_str())
            , static_cast<uint32>(value.size()));
    }
    return size;
}


//-----------------------------------------------------------------------------
uint32 File::write(const StaticString& value)
{
    uint32 size = 0;
    size += this->write<uint32>(static_cast<uint32>(value.size()));
    if (false == value.empty())
    {
        size += this->write(
              reinterpret_cast<const int8*>(value.c_str())
            , static_cast<uint32>(value.size()));
    }
    return size;
}


//-----------------------------------------------------------------------------
uint32 File::read(String& value_out)
{
    uint32 len = 0;
    this->read(len);
    value_out.resize(len);
    if (0 < len)
    {
        this->read(&value_out[0], len);
    }
    return sizeof(uint32) + len;
}


}
