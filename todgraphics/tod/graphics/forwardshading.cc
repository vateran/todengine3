#include "tod/random.h"
#include "tod/interpolation.h"
#include "tod/graphics/forwardshading.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/shadercache.h"
#include "tod/graphics/rendertarget.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/light.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/rendercomponent.h"
#include "tod/graphics/lightcomponent.h"
#include "tod/graphics/shadowcastercomponent.h"
#include "tod/graphics/cameracomponent.h"

namespace tod::graphics
{

//-----------------------------------------------------------------------------
ForwardShading::ForwardShading()
: renderer(nullptr)
, width(0)
, height(0)
{
}

//-----------------------------------------------------------------------------
void ForwardShading::init(Renderer* renderer, int width, int height)
{
    this->renderer = renderer;
    this->width = width;
    this->height = height;
    
    this->lightShader = ShaderCache::instance()->
        getShader("EngineData/glsl/forward_light.glvs",
                  "EngineData/glsl/forward_light.glfs");
    this->lightShader->begin();
    this->lightShader->setParam("texDiffuse", 0);
    this->lightShader->setParam("texDiffuse2", 3);
    this->lightShader->end();
}


//-----------------------------------------------------------------------------
void ForwardShading::render()
{
    this->render_cameras();
}


//-----------------------------------------------------------------------------
void ForwardShading::render_cameras()
{
    auto shader = this->lightShader.get();
    shader->begin();

    //모든 Camera view 로 렌더링
    for (auto& camera : this->renderer->renderCameras)
    {
        if (camera.shader)
        {
            camera.camera->clear();
        }
        
        auto proj_view = camera.transform->getWorldTransformMatrix();
        proj_view.inverse();
        proj_view *= camera.camera->getProjectionMatrix();
    
        for (auto& rc : this->renderer->renderCommands)
        {
            auto proj_view_world = rc.transform->getWorldTransformMatrix();
            proj_view_world *= proj_view;
            shader->setParam("matProjViewWorld", proj_view_world);
            rc.render->updateShaderParams(shader);
            shader->commitParams();
            
            
            //Render Object
            rc.render->render();
        }
    }
    
    shader->end();
}

}
