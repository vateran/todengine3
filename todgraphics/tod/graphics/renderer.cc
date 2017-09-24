#include "tod/graphics/renderer.h"
#include "tod/graphics/deferredshading.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/rendertarget.h"
#include "tod/graphics/shadercache.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/meshcomponent.h"
#include "tod/graphics/cameracomponent.h"
#include "tod/graphics/lightcomponent.h"
#include "tod/graphics/shadowcastercomponent.h"
#include "tod/graphics/primitivemesh.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
Renderer::Renderer():
shading(nullptr)
{
}


//-----------------------------------------------------------------------------
Renderer::~Renderer()
{
    SAFE_DELETE(this->deferredShading);
}


//-----------------------------------------------------------------------------
bool Renderer::initialize
(void* window_handle, int width, int height, bool windowed)
{
    //Engine 기본 Mesh 초기화
    PrimitiveMesh::instance();


    //Deferred Shading 초기화
    this->deferredShading = new DeferredShading();
    this->deferredShading->init(this, width, height);
    this->shading = this->deferredShading;
    
    
    //for Draw Texture
    this->quadMesh = this->createMesh(S("quad"));
    this->drawTextureShader = ShaderCache::instance()->
        getShader("EngineData/glsl/drawquad.glvs",
                  "EngineData/glsl/draw_texture.glfs");

    return true;
}


//-----------------------------------------------------------------------------
bool Renderer::render(Node* scene_root)
{
    //Transform와 BoudingVolume 을 업데이트, CameraComponent 도 수집
    Matrix44 t;
    t.identity();
    this->renderCommands.clear();
    this->renderLights.clear();
    this->renderCameras.clear();
    this->update_transform(scene_root, t, false);

    
    //지정된 방법으로 shading 수행
    this->shading->render();
    
    
    return true;
}



//-----------------------------------------------------------------------------
void Renderer::update_transform
(Node* current, const Matrix44& parent_transform, bool parent_transform_updated)
{
    if (!current->isVisible()) return;
    
    
    //TransformComponent로 WorldTransform 을 업데이트
    auto transform_compo = current->findComponent<TransformComponent>();
    bool transform_dirty = parent_transform_updated;
    if (transform_compo)
    {
        //transform 이 변경되었다면 업데이트
        transform_dirty = transform_dirty
            || transform_compo->isTransformDirty();
        if (transform_dirty)
            transform_compo->updateWorldTransform(parent_transform);
    }
    
    
    //ShaderComponent 가 있으면 shader stack 에 쌓는다
    auto shader_compo = current->findComponent<ShaderComponent>();
    if (nullptr != shader_compo)
        this->shaderStack.push(shader_compo);
    
    
    //RenderComponent 가 있으면 RenderCommand 에 넣는다
    auto render_compo = current->findComponent<RenderComponent>();
    if (nullptr != render_compo && nullptr != transform_compo)
    {
        RenderCommand render_command;
        render_command.node = current;
        render_command.transform = transform_compo;
        render_command.shader =
            this->shaderStack.empty()?nullptr:this->shaderStack.top();
        render_command.render = render_compo;
        this->renderCommands.emplace_back(render_command);
    }
    
    
    //Light 가 있으면 RenderLgiths 에 넣는다
    auto light_compo = current->findComponent<LightComponent>();
    auto shadow_caster_compo = current->findComponent<ShadowCasterComponent>();
    if (nullptr != light_compo && nullptr != transform_compo)
    {
        RenderLight render_light;
        render_light.transform = transform_compo;
        render_light.shader =
            this->shaderStack.empty()?nullptr:this->shaderStack.top();
        render_light.light = light_compo;
        render_light.shadowCaster = shadow_caster_compo;
        this->renderLights.emplace_back(render_light);
    }
    
    
    //Camera 가 있는지 검사
    auto camera_compo = current->findComponent<CameraComponent>();
    if (nullptr != camera_compo && nullptr != shader_compo)
    {
        RenderCamera render_camera;
        render_camera.transform = transform_compo;
        render_camera.shader = current->findComponent<ShaderComponent>();
        render_camera.camera = camera_compo;
        this->renderCameras.emplace_back(render_camera);
    }
    
    
    //자식 노드들의 transform 업데이트
    for (auto& child : current->getChildren())
    {
        this->update_transform(
            child,
            transform_compo!=nullptr?
                transform_compo->getWorldTransformMatrix()
                :parent_transform,
            transform_dirty);
    }
    
    if (!this->shaderStack.empty() && shader_compo)
        this->shaderStack.pop();
}


//-----------------------------------------------------------------------------
void Renderer::render_cameras()
{
    //여기서 renderCommands 를 texture, mesh, shader 로 정렬하자
    
    //전역 Camera 를 핸들링 하기 위한 shader 가 필요함
    
    //모든 Camera view 로 렌더링
    for (auto& camera : this->renderCameras)
    {
        if (camera.shader)
        {
            camera.camera->clear();
            camera.shader->begin();
            
            auto proj_view = camera.transform->getWorldTransformMatrix();
            proj_view.inverse();
            proj_view *= camera.camera->getProjectionMatrix();
            camera.shader->setParam("ProjViewMatrix", proj_view);
            camera.shader->setParam("ViewPos",
                camera.transform->getWorldTransformMatrix().getTranslation());
            camera.shader->commitParams();
        }
    
        for (auto& rc : this->renderCommands)
        {
            //Begin Shader
            if (rc.shader)
            {
                rc.shader->begin();
                rc.shader->setParam("WorldMatrix",
                    rc.transform->getWorldTransformMatrix());
                rc.shader->commitParams();
            }
            
            
            //Render Object
            rc.render->render();
            
            
            //End Shader
            if (rc.shader)
            {
                rc.shader->end();
            }
        }
        
        if (camera.shader) camera.shader->end();
    }
}


//-----------------------------------------------------------------------------
void Renderer::drawTexture(Texture* texture)
{
    this->drawTextureShader->begin();
    texture->use();
    this->drawTextureShader->setParam(S("Texture"), texture);
    this->quadMesh->render();
    this->drawTextureShader->end();
}


//-----------------------------------------------------------------------------
void Renderer::setSSAOKernelSize(int value)
{
    this->deferredShading->ssaoKernelSize = value;
    this->deferredShading->ssaoShader->setParam("KernelSize", value);
}


//-----------------------------------------------------------------------------
int Renderer::getSSAOKernelSize()
{
    return this->deferredShading->ssaoKernelSize;
}


//-----------------------------------------------------------------------------
void Renderer::setSSAOSampleRadius(float value)
{
    this->deferredShading->ssaoSampleRadius = value;
    this->deferredShading->ssaoShader->setParam("SampleRadius", value);
}


//-----------------------------------------------------------------------------
float Renderer::getSSAOSampleRadius()
{
    return this->deferredShading->ssaoSampleRadius;
}


//-----------------------------------------------------------------------------
void Renderer::setSSAOBias(float value)
{
    this->deferredShading->ssaoBias = value;
    this->deferredShading->ssaoShader->setParam("Bias", value);
}


//-----------------------------------------------------------------------------
float Renderer::getSSAOBias()
{
    return this->deferredShading->ssaoBias;
}


//-----------------------------------------------------------------------------
void Renderer::setSSAOPower(float value)
{
    this->deferredShading->ssaoPower = value;
    this->deferredShading->combineShader->setParam("SSAOPower", value);
}


//-----------------------------------------------------------------------------
float Renderer::getSSAOPower()
{
    return this->deferredShading->ssaoPower;
}


//-----------------------------------------------------------------------------
void Renderer::bindProperty()
{
    BIND_PROPERTY(int, "ssao_kernel_size", "ssao_kernel_size",
        setSSAOKernelSize, getSSAOKernelSize, 64, PropertyAttr::DEFAULT);
    BIND_PROPERTY(float, "ssao_sample_radius", "ssao_sample_radius",
        setSSAOSampleRadius, getSSAOSampleRadius, 0.5f, PropertyAttr::DEFAULT);
    BIND_PROPERTY(float, "ssao_bias", "ssao_bias",
        setSSAOBias, getSSAOBias, 0.025f, PropertyAttr::DEFAULT);
    BIND_PROPERTY(float, "ssao_power", "ssao_power",
        setSSAOPower, getSSAOPower, 2, PropertyAttr::DEFAULT);
}

    
}

