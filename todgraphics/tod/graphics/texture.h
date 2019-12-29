#pragma once
#include "tod/object.h"
#include "tod/graphics/graphicsdef.h"
#include "tod/graphics/colorf.h"
namespace tod::graphics
{

class Texture : public Derive<Texture, Object>
{
public:
    Texture();
    
    virtual bool create(int32 width, int32 height, Format format, void* data = nullptr);    
    virtual void destroy();
    
    virtual bool load(const String& uri, uint8 mipmap_level = 0);
    virtual void use(int32 sampler_index = 0);

    const String& getName() const;
    
    uint32 getWidth() const;
    uint32 getHeight() const;
    Format getFormat() const;
    
    virtual void setHorizontalWrap(TextureWrap value);
    virtual void setVerticalWrap(TextureWrap value);    
    virtual void setMinFilter(TextureFilter value);
    virtual void setMagFilter(TextureFilter value);
    virtual void setBorderColor(const ColorF& color);

    const ColorF& getBorderColor();
    static EnumList<int>& getWrapEnumerator();

    void setHorizontalWrap(int32 value);
    int32 getHorizontalWrap();
    void setVerticalWrap(int32 value);
    int32 getVerticalWrap();
    void setMinFilter(int32 value);
    int32 getMinFilter();    
    void setMagFilter(int32 value);
    int32 getMagFilter();
    
    static void bindProperty();
    
protected:
    void setName(const String& name);
    friend class Renderer;
    
protected:
    String name;
    uint32 width;
    uint32 height;
    Format format;
    TextureWrap hWrap;
    TextureWrap vWrap;
    TextureFilter minFilter;
    TextureFilter magFilter;
    ColorF borderColor;
    
};

}
