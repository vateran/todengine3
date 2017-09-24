#pragma once
#include "tod/object.h"
#include "tod/graphics/graphicsdef.h"
#include "tod/graphics/colorf.h"
namespace tod::graphics
{

class Texture : public Derive<Texture, Object>
{
public:
    Texture():
      width(0)
    , height(0)
    , format(PixelFormat::NONE)
    , hWrap(TextureWrap::REPEAT)
    , vWrap(TextureWrap::REPEAT)
    , minFilter(TextureFilter::NEAREST)
    , magFilter(TextureFilter::NEAREST)
    {
    }
    
    virtual bool create(int width, int height, PixelFormat format, void* data=nullptr)
    {
        TOD_RETURN_TRACE(false);
    }
    
    virtual void destroy()
    {
    }
    
    virtual bool load(const String& uri)
    {
        TOD_RETURN_TRACE(false);
    }
    
    virtual void use(int sampler_index=0)
    {
        
    }

    const String& getName() const { return this->name; }
    
    uint32 getWidth() const { return this->width; }
    uint32 getHeight() const { return this->height; }
    PixelFormat getFormat() const { return this->format; }
    
    virtual void setHorizontalWrap(TextureWrap value)
    {
        this->hWrap = value;
    }
    void setHorizontalWrap(int value)
    {
        this->setHorizontalWrap(static_cast<TextureWrap>(value));
    }
    int getHorizontalWrap()
    {
        return static_cast<int>(this->hWrap);
    }
    virtual void setVerticalWrap(TextureWrap value)
    {
        this->vWrap = value;
    }
    virtual void setVerticalWrap(int value)
    {
        this->setVerticalWrap(static_cast<TextureWrap>(value));
    }
    int getVerticalWrap()
    {
        return static_cast<int>(this->vWrap);
    }
    virtual void setMinFilter(TextureFilter value)
    {
        this->minFilter = value;
    }
    void setMinFilter(int value)
    {
        this->setMinFilter(static_cast<TextureFilter>(value));
    }
    int getMinFilter()
    {
        return static_cast<int>(this->minFilter);
    }
    virtual void setMagFilter(TextureFilter value)
    {
        this->magFilter = value;
    }
    void setMagFilter(int value)
    {
        this->setMagFilter(static_cast<TextureFilter>(value));
    }
    int getMagFilter()
    {
        return static_cast<int>(this->magFilter);
    }
    virtual void setBorderColor(const ColorF& color)
    {
        this->borderColor = color;
    }
    const ColorF& getBorderColor()
    {
        return this->borderColor;
    }
    static EnumList<int>& getWrapEnumerator()
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
    
    static void bindProperty()
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
    
protected:
    void setName(const String& name) { this->name = name; }
    friend class Renderer;
    
protected:
    String name;
    uint32 width;
    uint32 height;
    PixelFormat format;
    TextureWrap hWrap;
    TextureWrap vWrap;
    TextureFilter minFilter;
    TextureFilter magFilter;
    ColorF borderColor;
    
};

}
