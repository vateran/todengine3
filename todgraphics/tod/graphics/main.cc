#include "tod/platformdef.h"
#include "tod/graphics/config.h"
#ifdef PLATFORM_WINDOWS
#include "tod/graphics/dx12renderer.h"
#endif
#include "tod/graphics/openglrenderer.h"
namespace tod::graphics
{

void init()
{
    REGISTER_TYPE(tod::graphics::Renderer);
    REGISTER_TYPE(tod::graphics::Config);
    #ifdef PLATFORM_WINDOWS
    REGISTER_TYPE(tod::graphics::Dx12Renderer);
    #endif
    REGISTER_TYPE(tod::graphics::OpenGlRenderer);
}

}
