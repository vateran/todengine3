#pragma once
#include "tod/objref.h"
#include "tod/graphics/openglfunc.h"
#include "tod/graphics/texture.h"
namespace tod::graphics
{
    
class OpenGlTexture : public Derive<OpenGlTexture, Texture>
{
public:
    OpenGlTexture(const String& name=S(""));
    virtual~OpenGlTexture();
    
    bool create(int width, int height, PixelFormat format, void* data=nullptr) override;
    void destroy() override;
    bool load(const String& uri) override;
    
    void use(int sampler_index=0) override;
    
    void setHorizontalWrap(TextureWrap value) override;
    void setVerticalWrap(TextureWrap value) override;
    void setMinFilter(TextureFilter value) override;
    void setMagFilter(TextureFilter value) override;
    void setBorderColor(const ColorF& color) override;;
    
    GLuint getHandle() const { return this->texture; }
    GLint getSamplerIndex() const { return this->samplerIndex; }
    
private:
    bool format_to_glformat(
        PixelFormat format, GLint& gl_internal_format,
        GLenum& gl_format, GLenum& gl_type);
    
private:
    GLuint texture;
    GLint samplerIndex;

};
    
}
