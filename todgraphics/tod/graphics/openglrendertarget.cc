#include "tod/graphics/openglrendertarget.h"
#include "tod/graphics/opengltexture.h"
#include "tod/graphics/renderer.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
OpenGlRenderTarget::OpenGlRenderTarget(const String& name):
  renderer("/sys/renderer")
, width(0)
, height(0)
, fbo(0)
, rbo(0)
{
    this->name = name;
}


//-----------------------------------------------------------------------------
OpenGlRenderTarget::~OpenGlRenderTarget()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool OpenGlRenderTarget::create(int width, int height)
{
    //이전 texture 를 삭제
    this->destroy();

    
    //FrameBuffer 생성
    OpenGl::funcs->glGenFramebuffers(1, &this->fbo);
    TOD_OPENGL_CHECKERROR();
    
    
    //기본적으로 Color, Depth draw 가 모두 꺼져 있는 상태
    OpenGl::funcs->glDrawBuffers(0, nullptr);
    OpenGl::funcs->glReadBuffer(GL_NONE);
    
    
    //Depth RenderObject 생성
    OpenGl::funcs->glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    OpenGl::funcs->glGenRenderbuffers(1, &this->rbo);
    OpenGl::funcs->glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    OpenGl::funcs->glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    OpenGl::funcs->glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rbo);
    OpenGl::funcs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    this->width = width;
    this->height = height;
    
    
    return true;
}


//-----------------------------------------------------------------------------
Texture* OpenGlRenderTarget::addColorTarget(PixelFormat format)
{
    if (0 == this->fbo) TOD_RETURN_TRACE(nullptr);
    
    OpenGl::funcs->glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    
    auto texture = static_cast<OpenGlTexture*>
        (this->renderer->createTexture(
            String::fromFormat("%s_Color%d",
                &this->name[0],
                this->colorTextures.size())));
    if (!texture->create(this->width, this->height, format))
    {
        texture->release();
        TOD_RETURN_TRACE(nullptr);
    }
    texture->setMinFilter(TextureFilter::NEAREST);
    texture->setMagFilter(TextureFilter::NEAREST);
    texture->setHorizontalWrap(TextureWrap::CLAMP_TO_EDGE);
    texture->setVerticalWrap(TextureWrap::CLAMP_TO_EDGE);
    
    GLenum attachment = GL_COLOR_ATTACHMENT0
        + static_cast<int32>(this->colorTextures.size());
    
    OpenGl::funcs->glFramebufferTexture2D(
        GL_FRAMEBUFFER, attachment,
        GL_TEXTURE_2D, texture->getHandle(), 0);
    
    TOD_OPENGL_CHECKERROR();
    
    AttachInfo info;
    info.attachment = attachment;
    info.texture = texture;
    this->colorTextures.emplace_back(info);
    
    OpenGl::funcs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return texture;
}


//-----------------------------------------------------------------------------
Texture* OpenGlRenderTarget::makeDepthTarget(PixelFormat format)
{
    if (0 == this->fbo) TOD_RETURN_TRACE(nullptr);
    
    OpenGl::funcs->glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    
    if (this->rbo)
    {
        OpenGl::funcs->glDeleteRenderbuffers(1, &this->rbo);
        this->rbo = 0;
        OpenGl::funcs->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    
    auto texture = static_cast<OpenGlTexture*>
        (this->renderer->createTexture(
            String::fromFormat("%s_Depth", &this->name[0])));
    if (!texture->create(this->width, this->height, format))
    {
        texture->release();
        TOD_RETURN_TRACE(nullptr);
    }
    texture->setMinFilter(TextureFilter::NEAREST);
    texture->setMagFilter(TextureFilter::NEAREST);
    texture->setHorizontalWrap(TextureWrap::CLAMP_TO_EDGE);
    texture->setVerticalWrap(TextureWrap::CLAMP_TO_EDGE);
    
    OpenGl::funcs->glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, texture->getHandle(), 0);
    this->depthTexture = texture;
    TOD_OPENGL_CHECKERROR();
    
    OpenGl::funcs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return texture;
}


//-----------------------------------------------------------------------------
void OpenGlRenderTarget::blitColorTarget
(RenderTarget* to_rt, int from_index, int to_index)
{
    auto dst_rt = static_cast<OpenGlRenderTarget*>(to_rt);
    
    OpenGl::funcs->glViewport(0, 0, dst_rt->width, dst_rt->height);
    
    
    OpenGl::funcs->glBindFramebuffer(GL_READ_FRAMEBUFFER, this->fbo);
    OpenGl::funcs->glReadBuffer(GL_COLOR_ATTACHMENT0 + from_index);
    
    
    OpenGl::funcs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_rt->fbo);
    GLenum attachments[1] = { static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + to_index) };
    OpenGl::funcs->glDrawBuffers(1, attachments);
    
    
    OpenGl::funcs->glBlitFramebuffer(
        0, 0, this->width, this->height,
        0, 0, dst_rt->width, dst_rt->height,
        GL_COLOR_BUFFER_BIT, GL_LINEAR);
    //TOD_OPENGL_CHECKERROR();
    
    
    OpenGl::funcs->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    OpenGl::funcs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


//-----------------------------------------------------------------------------
Texture* OpenGlRenderTarget::getColorTexture(int index)
{
    return this->colorTextures[index].texture;
}


//-----------------------------------------------------------------------------
Texture* OpenGlRenderTarget::getDepthTexture()
{
    return this->depthTexture;
}


//-----------------------------------------------------------------------------
void OpenGlRenderTarget::destroy()
{
    if (0 == this->rbo)
    {
        OpenGl::funcs->glDeleteRenderbuffers(1, &this->rbo);
        this->rbo = 0;
    }

    if (0 != this->fbo)
    {
        OpenGl::funcs->glDeleteFramebuffers(1, &this->fbo);
        this->fbo = 0;
    }
}


//-----------------------------------------------------------------------------
void OpenGlRenderTarget::begin(bool clear_color, bool clear_depth)
{
    if (0 == this->fbo) return;
    
    OpenGl::funcs->glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    OpenGl::funcs->glViewport(0, 0, this->width, this->height);

    if (colorTextures.empty())
    {
        OpenGl::funcs->glDrawBuffers(0, nullptr);
    }
    else
    {
        std::vector<uint32> attachments;
        attachments.reserve(this->colorTextures.size());
        for (int i=0;i<this->colorTextures.size();++i)
            attachments.push_back(this->colorTextures[i].attachment);

        OpenGl::funcs->glDrawBuffers(
            static_cast<GLsizei>(attachments.size()),
            &attachments[0]);
    }
    
    if (OpenGl::funcs->glCheckFramebufferStatus(GL_FRAMEBUFFER)
        != GL_FRAMEBUFFER_COMPLETE)
        TOD_THROW_EXCEPTION("Framebuffer not complete!");
    
    GLbitfield mask = 0;
    if (clear_color) mask |= GL_COLOR_BUFFER_BIT;
    if (clear_depth) mask |= GL_DEPTH_BUFFER_BIT;
    if (mask > 0) OpenGl::funcs->glClear(mask);
}


//-----------------------------------------------------------------------------
void OpenGlRenderTarget::end()
{
    OpenGl::funcs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//-----------------------------------------------------------------------------
void OpenGlRenderTarget::use()
{
    int sampler_index = 0;
    for (auto& i : this->colorTextures)
    {
        i.texture->use(sampler_index++);
    }
    if (this->depthTexture.valid())
        this->depthTexture->use(sampler_index);
}

}
