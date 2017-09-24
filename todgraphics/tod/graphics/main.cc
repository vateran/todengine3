#include "tod/platformdef.h"
#include "tod/graphics/config.h"
#ifdef PLATFORM_WINDOWS
#include "tod/graphics/dx12renderer.h"
#endif
#include "tod/graphics/openglrenderer.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/meshcomponent.h"
#include "tod/graphics/cameracomponent.h"
#include "tod/graphics/lightcomponent.h"
#include "tod/graphics/shadowcastercomponent.h"
#include "tod/graphics/modelcomponent.h"
#include "tod/graphics/oceancomponent.h"
#include "tod/graphics/heightfieldterraincomponent.h"
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
    
    REGISTER_TYPE(tod::graphics::TransformComponent);
    REGISTER_TYPE(tod::graphics::RenderComponent);
    REGISTER_TYPE(tod::graphics::MeshComponent);
    REGISTER_TYPE(tod::graphics::ShaderComponent);
    REGISTER_TYPE(tod::graphics::CameraComponent);
    REGISTER_TYPE(tod::graphics::LightComponent);
    REGISTER_TYPE(tod::graphics::ShadowCasterComponent);
    REGISTER_TYPE(tod::graphics::ModelComponent);
    REGISTER_TYPE(tod::graphics::OceanComponent);
    REGISTER_TYPE(tod::graphics::HeightFieldTerrainComponent);
}

}
