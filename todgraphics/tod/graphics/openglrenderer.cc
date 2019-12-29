#include "tod/platformdef.h"
#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include "tod/graphics/openglrenderer.h"
#include "tod/graphics/openglshader.h"
#include "tod/graphics/openglmesh.h"
#include "tod/graphics/opengltexture.h"
#include "tod/graphics/openglrendertarget.h"
#include "tod/graphics/openglcamera.h"
#include "tod/graphics/modelcomponent.h"

namespace tod::graphics
{


//-----------------------------------------------------------------------------
OpenGl::FuncsType* OpenGl::funcs = nullptr;
    

//-----------------------------------------------------------------------------
void OpenGl::checkError()
{
    auto err = OpenGl::funcs->glGetError();
    switch (err)
    {
    case GL_NO_ERROR: break;
    case GL_INVALID_ENUM:
        printf("OpenGlError:GL_INVALID_ENUM\n");
        break;
    case GL_INVALID_OPERATION:
        printf("OpenGlError:GL_INVALID_OPERATION\n");
        break;
    case GL_INVALID_VALUE:
        printf("OpenGlError:GL_INVALID_VALUE\n");
        break;
    case GL_STACK_OVERFLOW:
        printf("OpenGlError:GL_STACK_OVERFLOW\n");
        break;
    case GL_STACK_UNDERFLOW:
        printf("OpenGlError:GL_STACK_UNDERFLOW\n");
        break;
    case GL_OUT_OF_MEMORY:
        printf("OpenGlError:OpenGlError\n");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        printf("OpenGlError:GL_INVALID_FRAMEBUFFER_OPERATION\n");
        break;
    }
}
    
    
//-----------------------------------------------------------------------------
bool OpenGlRenderer::initialize
(void* context, int width, int height, bool windowed)
{
    OpenGl::funcs = static_cast<OpenGl::FuncsType*>(context);
    
    const auto renderer = OpenGl::funcs->glGetString(GL_RENDERER);
    const auto version = OpenGl::funcs->glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    OpenGl::funcs->glEnable(GL_DEPTH_TEST);
    OpenGl::funcs->glDepthFunc(GL_LESS);
    
    return BaseType::initialize(context, width, height, windowed);
}


//-----------------------------------------------------------------------------
void OpenGlRenderer::setViewport(int width, int height)
{
    OpenGl::funcs->glViewport(0, 0, width, height);
}


//-----------------------------------------------------------------------------
void OpenGlRenderer::enableZTest(bool value)
{
    if (value)
        OpenGl::funcs->glEnable(GL_DEPTH_TEST);
    else
        OpenGl::funcs->glDisable(GL_DEPTH_TEST);
}


//-----------------------------------------------------------------------------
void OpenGlRenderer::enableAlphaTest(bool value)
{
    if (value)
        OpenGl::funcs->glEnable(GL_ALPHA_TEST);
    else
        OpenGl::funcs->glDisable(GL_ALPHA_TEST);
}


//-----------------------------------------------------------------------------
void OpenGlRenderer::cullFace(CullFace value)
{
    switch (value)
    {
    case CullFace::FRONT:
        OpenGl::funcs->glCullFace(GL_FRONT);
        break;
    case CullFace::BACK:
        OpenGl::funcs->glCullFace(GL_BACK);
        break;
    }
}


//-----------------------------------------------------------------------------
Shader* OpenGlRenderer::createShader()
{
    return new OpenGlShader;
}


//-----------------------------------------------------------------------------
Mesh* OpenGlRenderer::createMesh(const String& name)
{
    if (!name.empty())
    {
        auto i = this->namedMeshes.find(name.hash());
        if (this->namedMeshes.end() != i)
        {
            return i->second;
        }
    }

    auto mesh = new OpenGlMesh(name);
    
    if (!name.empty())
    {
        this->namedMeshes[name.hash()] = mesh;
    }
 
    return mesh;
}


//-----------------------------------------------------------------------------
Texture* OpenGlRenderer::createTexture(const String& name)
{
    if (!name.empty())
    {
        auto i = this->namedTextures.find(name.hash());
        if (this->namedTextures.end() != i)
        {
            return i->second;
        }
    }

    auto texture = new OpenGlTexture(name);
    
    if (!name.empty())
    {
        this->namedTextures[name.hash()] = texture;
    }
 
    return texture;
}


//-----------------------------------------------------------------------------
RenderTarget* OpenGlRenderer::createRenderTarget(const String& name)
{
    return new OpenGlRenderTarget(name);
}


//-----------------------------------------------------------------------------
Camera* OpenGlRenderer::createCamera()
{
    return new OpenGlCamera;
}

}
#endif