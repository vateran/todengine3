#pragma once
#include "tod/objref.h"
#include "tod/graphics/openglfunc.h"
#include "tod/graphics/rendertarget.h"
namespace tod::graphics
{

class Renderer;
class OpenGlTexture;
class OpenGlRenderTarget : public Derive<OpenGlRenderTarget, RenderTarget>
{
public:
    OpenGlRenderTarget(const String& name=S(""));
    virtual~OpenGlRenderTarget();
    
    bool create(int width, int height) override;
    void destroy() override;
    
    Texture* addColorTarget(PixelFormat format) override;
    Texture* makeDepthTarget(PixelFormat format) override;
    void blitColorTarget(RenderTarget* to_rt, int from_index, int to_index) override;
    Texture* getColorTexture(int index) override;
    Texture* getDepthTexture() override;
    
    void begin(bool clear_color, bool clear_depth) override;
    void end() override;
    
    void use() override;
    
private:
    typedef std::tuple<int, int, PixelFormat> Key;
    
    ObjRef<Renderer> renderer;
    int width;
    int height;
    GLuint fbo;
    GLuint rbo;
    struct AttachInfo
    {
        GLenum attachment;
        PixelFormat format;
        ObjRef<OpenGlTexture> texture;
        float scale;
    };
    std::vector<AttachInfo> colorTextures;
    ObjRef<OpenGlTexture> depthTexture;
    
};

}

