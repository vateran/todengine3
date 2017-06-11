#include "tod/graphics/dx12renderer.h"
#include "tod/graphics/openglrenderer.h"
namespace tod::graphics
{

void init()
{
    REGISTER_TYPE(tod::graphics::Renderer);
    REGISTER_TYPE(tod::graphics::Dx12Renderer);
    REGISTER_TYPE(tod::graphics::OpenGlRenderer);
}

}