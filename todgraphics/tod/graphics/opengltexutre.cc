#include "tod/graphics/opengltexture.h"
#include "tod/filesystem.h"
#include "FreeImage.h"
namespace tod::graphics
{

class MemIO : public FreeImageIO
{
public :
    MemIO(char* data) : start(data), cp(data)
    {
        read_proc  = readProc;
        write_proc = writeProc;
        tell_proc  = tellProc;
        seek_proc  = seekProc;
    }

    void Reset()
    {
		cp = start;
	}

private:
    static unsigned readProc(void* buffer, unsigned size, unsigned count, fi_handle handle);
    static unsigned writeProc(void* buffer, unsigned size, unsigned count, fi_handle handle);
    static int seekProc(fi_handle handle, long offset, int origin);
	static long tellProc(fi_handle handle);

private:
    char* start;
    char* cp;
};


//-----------------------------------------------------------------------------
unsigned MemIO::readProc
(void* buffer, unsigned size, unsigned count, fi_handle handle)
{
    auto mem_io = static_cast<MemIO*>(handle);
    auto tmp = static_cast<BYTE*>(buffer);

    for (unsigned c = 0; c < count; c++)
    {
        memcpy(tmp, mem_io->cp, size);
        mem_io->cp = mem_io->cp + size;
        tmp += size;
    }
    return count;
}


//-----------------------------------------------------------------------------
unsigned MemIO::writeProc
(void* buffer, unsigned size, unsigned count, fi_handle handle)
{
    return size;
}


//-----------------------------------------------------------------------------
int MemIO::seekProc(fi_handle handle, long offset, int origin)
{
    auto mem_io = static_cast<MemIO*>(handle);

    if (origin == SEEK_SET) 
        mem_io->cp = mem_io->start + offset;
    else
        mem_io->cp = mem_io->cp + offset;

    return 0;
}


//-----------------------------------------------------------------------------
long MemIO::tellProc(fi_handle handle)
{
    auto mem_io = static_cast<MemIO*>(handle);
    return mem_io->cp - mem_io->start;
}


//-----------------------------------------------------------------------------
OpenGlTexture::OpenGlTexture(const String& name):
  texture(0)
, samplerIndex(0)
{
    this->setName(name);
}


//-----------------------------------------------------------------------------
OpenGlTexture::~OpenGlTexture()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool OpenGlTexture::create
(int width, int height, PixelFormat format, void* data)
{
    //이전 texture 를 삭제
    this->destroy();
    
    
    //GL texture format 을 알아낸다
    GLint gl_internal_format;
    GLenum gl_format, gl_type;
    if (!this->format_to_glformat(
        format, gl_internal_format, gl_format, gl_type))
        TOD_RETURN_TRACE(false);
    

    //Texture 생성
    OpenGl::funcs->glGenTextures(1, &this->texture);
    this->use();
    OpenGl::funcs->glTexImage2D(
        GL_TEXTURE_2D, 0, gl_internal_format,
        width, height, 0, gl_format, gl_type, data);
    TOD_OPENGL_CHECKERROR();
    
    
    this->width = width;
    this->height = height;
    this->format = format;
    
    return true;
}


//-----------------------------------------------------------------------------
bool OpenGlTexture::format_to_glformat
(PixelFormat format, GLint& gl_internal_format,
 GLenum& gl_format, GLenum& gl_type)
{
    switch (format)
    {
    case PixelFormat::L8F:
        gl_internal_format = GL_RED;
        gl_format = GL_RGB;
        gl_type = GL_FLOAT;
        break;
    case PixelFormat::R8G8B8:
        gl_internal_format = GL_RGB;
        gl_format = GL_BGR;
        gl_type = GL_UNSIGNED_BYTE;
        break;
    case PixelFormat::R8G8B8A8:
        gl_internal_format = GL_RGBA;
        gl_format = GL_BGRA;
        gl_type = GL_UNSIGNED_BYTE;
        break;
    case PixelFormat::R16G16B16F:
        gl_internal_format = GL_RGB16F;
        gl_format = GL_RGB;
        gl_type = GL_FLOAT;
        break;
    case PixelFormat::R16G16B16A16F:
        gl_internal_format = GL_RGBA16F;
        gl_format = GL_RGBA;
        gl_type = GL_FLOAT;
        break;
    case PixelFormat::DEPTH16:
        gl_internal_format = GL_DEPTH_COMPONENT;
        gl_format = GL_DEPTH_COMPONENT;
        gl_type = GL_FLOAT;
        break;
    case PixelFormat::DEPTH32F:
        gl_internal_format = GL_DEPTH_COMPONENT32F;
        gl_format = GL_DEPTH_COMPONENT;
        gl_type = GL_FLOAT;
        break;
    default:
        TOD_RETURN_TRACE(false);
    }
    return true;
}


//-----------------------------------------------------------------------------
void OpenGlTexture::destroy()
{
    if (0 != this->texture)
    {
        OpenGl::funcs->glDeleteTextures(1, &this->texture);
        this->texture = 0;
    }
}


//-----------------------------------------------------------------------------
void OpenGlTexture::use(int sampler_index)
{
    if (0 == this->texture) return;
    
    this->samplerIndex = sampler_index;
    OpenGl::funcs->glActiveTexture(this->samplerIndex + GL_TEXTURE0);
    OpenGl::funcs->glBindTexture(GL_TEXTURE_2D, this->texture);
}


//-----------------------------------------------------------------------------
bool OpenGlTexture::load(const String& uri)
{
    FileSystem::Data img_data;
    if (!FileSystem::instance()->load(uri, img_data))
        TOD_RETURN_TRACE(false);
    
    MemIO mem_io(&img_data[0]);
    
    auto fif = FreeImage_GetFileTypeFromHandle(
        &mem_io,
        static_cast<fi_handle>(&mem_io), 0);
    auto raw_img = FreeImage_LoadFromHandle(fif, &mem_io,
        static_cast<fi_handle>(&mem_io));
    
    FIBITMAP* img = nullptr;
    PixelFormat texture_format;
    switch (FreeImage_GetColorType(raw_img))
    {
    case FIC_RGB:
        texture_format = PixelFormat::R8G8B8;
        img = FreeImage_ConvertTo24Bits(raw_img);
        break;
	case FIC_RGBALPHA:
        texture_format = PixelFormat::R8G8B8A8;
        img = FreeImage_ConvertTo32Bits(raw_img);
        break;
    default:
        FreeImage_Unload(raw_img);
        TOD_RETURN_TRACE(false);
    };
    
    if (nullptr == img)
        TOD_RETURN_TRACE(false);
    
    auto width = FreeImage_GetWidth(img);
	auto height = FreeImage_GetHeight(img);
    auto bits = FreeImage_GetBits(img);
    
    this->create(width, height, texture_format, bits);
    this->setHorizontalWrap(TextureWrap::REPEAT);
    this->setVerticalWrap(TextureWrap::REPEAT);
    this->setMinFilter(TextureFilter::LINEAR);
    this->setMagFilter(TextureFilter::LINEAR);
    
    FreeImage_Unload(img);
    return true;
}


//-----------------------------------------------------------------------------
void OpenGlTexture::setHorizontalWrap(TextureWrap value)
{
    GLint gl_value = 0;
    switch (value)
    {
    case TextureWrap::REPEAT:
        gl_value = GL_REPEAT;
        break;
    case TextureWrap::MIRRORED_REPEAT:
        gl_value = GL_MIRRORED_REPEAT;
        break;
    case TextureWrap::CLAMP_TO_EDGE:
        gl_value = GL_CLAMP_TO_EDGE;
        break;
    case TextureWrap::CLAMP_TO_BORDER:
        gl_value = GL_CLAMP_TO_BORDER;
        break;
    }

    BaseType::setHorizontalWrap(value);
    OpenGl::funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_value);
}


//-----------------------------------------------------------------------------
void OpenGlTexture::setVerticalWrap(TextureWrap value)
{
    GLint gl_value = 0;
    switch (value)
    {
    case TextureWrap::REPEAT:
        gl_value = GL_REPEAT;
        break;
    case TextureWrap::MIRRORED_REPEAT:
        gl_value = GL_MIRRORED_REPEAT;
        break;
    case TextureWrap::CLAMP_TO_EDGE:
        gl_value = GL_CLAMP_TO_EDGE;
        break;
    case TextureWrap::CLAMP_TO_BORDER:
        gl_value = GL_CLAMP_TO_BORDER;
        break;
    }

    BaseType::setVerticalWrap(value);
    OpenGl::funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_value);
}


//-----------------------------------------------------------------------------
void OpenGlTexture::setMinFilter(TextureFilter value)
{
    GLint gl_value = 0;
    switch (value)
    {
    case TextureFilter::NEAREST:
        gl_value = GL_NEAREST;
        break;
    case TextureFilter::LINEAR:
        gl_value = GL_LINEAR;
        break;
    }

    BaseType::setMinFilter(value);
    OpenGl::funcs->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_value);
}


//-----------------------------------------------------------------------------
void OpenGlTexture::setMagFilter(TextureFilter value)
{
    GLint gl_value = 0;
    switch (value)
    {
    case TextureFilter::NEAREST:
        gl_value = GL_NEAREST;
        break;
    case TextureFilter::LINEAR:
        gl_value = GL_LINEAR;
        break;
    }
    
    BaseType::setMagFilter(value);
    OpenGl::funcs->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_value);
}


//-----------------------------------------------------------------------------
void OpenGlTexture::setBorderColor(const ColorF& color)
{
    BaseType::setBorderColor(color);
    
    OpenGl::funcs->glTexParameterfv(
        GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color.array);
}

}
