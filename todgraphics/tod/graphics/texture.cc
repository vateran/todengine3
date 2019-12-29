#include "tod/graphics/texture.h"
namespace tod::graphics
{

//----------------------------------------------------------------------------
Texture::Texture()
    : width(0)
    , height(0)
    , format(Format::NONE)
    , hWrap(TextureWrap::REPEAT)
    , vWrap(TextureWrap::REPEAT)
    , minFilter(TextureFilter::NEAREST)
    , magFilter(TextureFilter::NEAREST)
{
}


//----------------------------------------------------------------------------
bool Texture::create(int32 width, int32 height, Format format, void* data)
{
    TOD_RETURN_TRACE(false);
}


//----------------------------------------------------------------------------
void Texture::destroy()
{
}


//----------------------------------------------------------------------------
bool Texture::load(const String& uri, uint8 mip_level)
{
    TOD_RETURN_TRACE(false);
}


//----------------------------------------------------------------------------
void Texture::use(int32 sampler_index)
{
}


//----------------------------------------------------------------------------
const String& Texture::getName() const
{
    return this->name;
}


//----------------------------------------------------------------------------
uint32 Texture::getWidth() const
{
    return this->width;
}


//----------------------------------------------------------------------------
uint32 Texture::getHeight() const
{
    return this->height;
}


//----------------------------------------------------------------------------
Format Texture::getFormat() const
{
    return this->format;
}


//----------------------------------------------------------------------------
void Texture::setHorizontalWrap(TextureWrap value)
{
    this->hWrap = value;
}


//----------------------------------------------------------------------------
void Texture::setHorizontalWrap(int32 value)
{
    this->setHorizontalWrap(static_cast<TextureWrap>(value));
}


//----------------------------------------------------------------------------
int32 Texture::getHorizontalWrap()
{
    return static_cast<int>(this->hWrap);
}


//----------------------------------------------------------------------------
void Texture::setVerticalWrap(TextureWrap value)
{
    this->vWrap = value;
}


//----------------------------------------------------------------------------
void Texture::setVerticalWrap(int32 value)
{
    this->setVerticalWrap(static_cast<TextureWrap>(value));
}


//----------------------------------------------------------------------------
int32 Texture::getVerticalWrap()
{
    return static_cast<int>(this->vWrap);
}


//----------------------------------------------------------------------------
void Texture::setMinFilter(TextureFilter value)
{
    this->minFilter = value;
}


//----------------------------------------------------------------------------
void Texture::setMinFilter(int32 value)
{
    this->setMinFilter(static_cast<TextureFilter>(value));
}


//----------------------------------------------------------------------------
int32 Texture::getMinFilter()
{
    return static_cast<int>(this->minFilter);
}


//----------------------------------------------------------------------------
void Texture::setMagFilter(TextureFilter value)
{
    this->magFilter = value;
}


//----------------------------------------------------------------------------
void Texture::setMagFilter(int32 value)
{
    this->setMagFilter(static_cast<TextureFilter>(value));
}


//----------------------------------------------------------------------------
int32 Texture::getMagFilter()
{
    return static_cast<int>(this->magFilter);
}


//----------------------------------------------------------------------------
void Texture::setBorderColor(const ColorF& color)
{
    this->borderColor = color;
}


//----------------------------------------------------------------------------
const ColorF& Texture::getBorderColor()
{
    return this->borderColor;
}


//----------------------------------------------------------------------------
EnumList<int>& Texture::getWrapEnumerator()
{
    static EnumList<int> s_enum
    {
        std::make_tuple("Repeat", static_cast<int>(TextureWrap::REPEAT)),
        std::make_tuple("MirroredRepeat", static_cast<int>(TextureWrap::MIRRORED_REPEAT)),
        std::make_tuple("ClampToEdge", static_cast<int>(TextureWrap::CLAMP_TO_EDGE)),
        std::make_tuple("ClampToBorder", static_cast<int>(TextureWrap::CLAMP_TO_BORDER)),
    };
    return s_enum;
}


//----------------------------------------------------------------------------
void Texture::bindProperty()
{
    BIND_ENUM_PROPERTY(int, "horizontal_wrap", "horizontal_wrap",
        setHorizontalWrap, getHorizontalWrap,
        getWrapEnumerator,
        static_cast<int>(TextureWrap::REPEAT),
        PropertyAttr::DEFAULT);
    BIND_ENUM_PROPERTY(int, "vertical_wrap", "vertical_wrap",
        setVerticalWrap, getVerticalWrap,
        getWrapEnumerator,
        static_cast<int>(TextureWrap::REPEAT),
        PropertyAttr::DEFAULT);
}


//----------------------------------------------------------------------------
void Texture::setName(const String& name)
{
    this->name = name;
}

}
