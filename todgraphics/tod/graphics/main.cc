#include "tod/platformdef.h"
#include "tod/graphics/config.h"
#ifdef TOD_PLATFORM_WINDOWS
//#include "tod/graphics/dx11renderer.h"
#else
#include "tod/graphics/openglrenderer.h"
#endif
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/meshcomponent.h"
#include "tod/graphics/cameracomponent.h"
#include "tod/graphics/animcomponent.h"
#include "tod/graphics/lightcomponent.h"
#include "tod/graphics/shadowcastercomponent.h"
#include "tod/graphics/modelcomponent.h"
#include "tod/graphics/oceancomponent.h"
#include "tod/graphics/heightfieldterraincomponent.h"
namespace tod::graphics
{

void init()
{
    //REGISTER_TYPE(tod::graphics::Renderer);
    REGISTER_TYPE(tod::graphics::Config);
    #ifdef TOD_PLATFORM_WINDOWS
    //REGISTER_TYPE(tod::graphics::Dx11Renderer);
    #else
    REGISTER_TYPE(tod::graphics::OpenGlRenderer);
    #endif
    
    REGISTER_TYPE(tod::graphics::TransformComponent);
    REGISTER_TYPE(tod::graphics::RenderComponent);
    REGISTER_TYPE(tod::graphics::MeshComponent);
    REGISTER_TYPE(tod::graphics::ShaderComponent);
    REGISTER_TYPE(tod::graphics::AnimComponent);
    REGISTER_TYPE(tod::graphics::CameraComponent);
    REGISTER_TYPE(tod::graphics::LightComponent);
    REGISTER_TYPE(tod::graphics::ShadowCasterComponent);
}

}
