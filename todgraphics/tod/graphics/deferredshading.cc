#include "tod/random.h"
#include "tod/interpolation.h"
#include "tod/graphics/deferredshading.h"
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
DeferredShading::DeferredShading()
: renderer(nullptr)
, ssaoKernelSize(64)
, ssaoSampleRadius(3)
, ssaoBias(0.1f)
, ssaoPower(1)
{
}


//-----------------------------------------------------------------------------
void DeferredShading::init(Renderer* renderer, int width, int height)
{
    this->renderer = renderer;
    this->width = width;
    this->height = height;
    

    //Geometry pass shader
    this->geometryPassShader = ShaderCache::instance()->
        getShader("EngineData/glsl/deferred_geometry_pass.glvs",
                  "EngineData/glsl/deferred_geometry_pass.glfs");
    this->geometryPassShader->begin();
    this->geometryPassShader->setParam("DiffuseTex", 0);
    this->geometryPassShader->setParam("SpecularTex", 1);
    this->geometryPassShader->setParam("NormalTex", 2);
    this->geometryPassShader->end();
    
    
    //Light pass shader
    this->lightPassShader = ShaderCache::instance()->
        getShader("EngineData/glsl/deferred_light_pass.glvs",
                  "EngineData/glsl/deferred_light_pass.glfs");
    this->lightPassShader->begin();
    this->lightPassShader->setParam("NormalTex", 0);
    this->lightPassShader->setParam("AlbedoSpecularTex", 1);
    this->lightPassShader->setParam("DepthTex", 2);
    this->lightPassShader->setParam("ShadowMapTex", 3);
    this->lightPassShader->end();
    


    //G-Buffer
    this->gBuffer = this->renderer->createRenderTarget(S("G-Buffer"));
    this->gBuffer->create(width, height);
    this->gBuffer->addColorTarget(PixelFormat::R16G16B16F);
    this->gBuffer->addColorTarget(PixelFormat::R8G8B8A8);
    this->gBuffer->makeDepthTarget(PixelFormat::DEPTH32F);
    
    
    this->quadMesh = this->renderer->createMesh(S("quad"));
    
    
    //Blur pass shader
    this->blurShader = ShaderCache::instance()->
        getShader("EngineData/glsl/drawquad.glvs",
                  "EngineData/glsl/blur.glfs");
    
    
    //HDR buffer
    this->hdrBuffer = this->renderer->createRenderTarget(S("HDR"));
    this->hdrBuffer->create(width, height);
    this->hdrBuffer->addColorTarget(PixelFormat::R16G16B16F);
    this->hdrBuffer->addColorTarget(PixelFormat::R8G8B8A8);
    
    
    //SSAO pass shader
    this->ssaoShader = ShaderCache::instance()->
        getShader("EngineData/glsl/SSAO.glvs",
                  "EngineData/glsl/SSAO.glfs");
    this->ssaoShader->begin();
    Random r;
    for (int i=0;i<64;++i)
    {
        Vector3 sample(
            r.uniformFloat(0, 1) * 2 - 1,
            r.uniformFloat(0, 1) * 2 - 1,
            r.uniformFloat(0, 1));
        sample.normalize();
        sample *= r.uniformFloat(0, 1);
        
        float scale = i / 64.0f;
        scale = Interpolation::linear(scale * scale, 0.1f, 1.0f);
        sample *= scale;
        this->ssaoShader->setParam(
            String::fromFormat("Samples[%d]", i), sample);
    }
    
    this->ssaoShader->setParam("KernelSize", this->ssaoKernelSize);
    this->ssaoShader->setParam("SampleRadius", this->ssaoSampleRadius);
    this->ssaoShader->setParam("Bias", this->ssaoBias);
    this->ssaoShader->setParam("NormalTex", 0);
    this->ssaoShader->setParam("NoiseTex", 1);
    this->ssaoShader->setParam("DepthTex", 2);
    this->ssaoShader->end();
    
    
    //SSAO blur pass shader
    this->ssaoBlurShader = ShaderCache::instance()->
        getShader("EngineData/glsl/drawquad.glvs",
                  "EngineData/glsl/SSAO_blur.glfs");
    this->ssaoBlurShader->begin();
    this->ssaoBlurShader->setParam("SSAOTex", 0);
    this->ssaoBlurShader->end();
    
    
    //SSAO buffer
    this->ssaoBuffer = this->renderer->createRenderTarget(S("SSAO"));
    this->ssaoBuffer->create(width/4, height/4);
    this->ssaoBuffer->addColorTarget(PixelFormat::L8F);
    
    
    //SSAO blur buffer
    this->ssaoBlurBuffer = this->renderer->createRenderTarget(S("SSAO-Blur"));
    this->ssaoBlurBuffer->create(width/8, height/8);
    auto ssao_blur_buffer = this->ssaoBlurBuffer->addColorTarget(PixelFormat::L8F);
    ssao_blur_buffer->setMagFilter(TextureFilter::LINEAR);
    
    
    //make noise texture
    std::vector<Vector3> noise_data;
    noise_data.reserve(4 * 4);
    for (int i=0;i<noise_data.capacity();++i)
    {
        noise_data.push_back(Vector3(
            r.uniformFloat(0, 1) * 2 - 1,
            r.uniformFloat(0, 1) * 2 - 1,
            0));
    }
    this->noiseTexture = this->renderer->createTexture("Noise4x4");
    this->noiseTexture->create(4, 4, PixelFormat::R16G16B16F, &noise_data[0]);
    this->noiseTexture->setHorizontalWrap(TextureWrap::REPEAT);
    this->noiseTexture->setVerticalWrap(TextureWrap::REPEAT);
    this->noiseTexture->setMinFilter(TextureFilter::NEAREST);
    this->noiseTexture->setMagFilter(TextureFilter::NEAREST);
    
    
    
    
    for (int i=0;i<2;++i)
    {
        this->bloomBuffer[i] = this->renderer->
            createRenderTarget(String::fromFormat(S("%s%d"), S("Bloom"), i));
        this->bloomBuffer[i]->create(width / 4, height / 4);
        auto colot_tex = this->bloomBuffer[i]->addColorTarget(PixelFormat::R8G8B8A8);
        colot_tex->setMinFilter(TextureFilter::LINEAR);
        colot_tex->setMagFilter(TextureFilter::LINEAR);
        colot_tex->setHorizontalWrap(TextureWrap::CLAMP_TO_EDGE);
        colot_tex->setVerticalWrap(TextureWrap::CLAMP_TO_EDGE);
    }
    
    
    
    //Compine pass shader
    this->combineShader = ShaderCache::instance()->
        getShader("EngineData/glsl/combine_pass.glvs",
                  "EngineData/glsl/combine_pass.glfs");
    this->combineShader->begin();
    this->combineShader->setParam("Scene", 0);
    this->combineShader->setParam("Bloom", 1);
    this->combineShader->setParam("SSAO", 2);
    this->combineShader->setParam("Blur", 3);
    this->combineShader->setParam("Depth", 4);
    this->combineShader->setParam("SSAOPower", this->ssaoPower);
    this->combineShader->end();
    
    
    
    //Combine Buffer
    this->combineBuffer = this->renderer->
            createRenderTarget("Combine");
    this->combineBuffer->create(width, height);
    this->combineBuffer->addColorTarget(PixelFormat::R8G8B8);

    
    
    for (int i=0;i<2;++i)
    {
        this->dofBlurBuffer[i] = this->renderer->
            createRenderTarget(String::fromFormat(S("%s%d"), S("DOFBlur"), i));
        this->dofBlurBuffer[i]->create(width/2, height/2);
        auto colot_tex = this->dofBlurBuffer[i]->addColorTarget(PixelFormat::R8G8B8);
        colot_tex->setMinFilter(TextureFilter::LINEAR);
        colot_tex->setMagFilter(TextureFilter::LINEAR);
        colot_tex->setHorizontalWrap(TextureWrap::CLAMP_TO_EDGE);
        colot_tex->setVerticalWrap(TextureWrap::CLAMP_TO_EDGE);
    }
}


//-----------------------------------------------------------------------------
void DeferredShading::render()
{
    this->renderer->enableZTest(true);
    this->renderer->enableAlphaTest(false);

    //1.Geometry Pass
    //모든 Camera 를 렌더링
    this->render_cameras();
    
    
    this->render_shadows();
    
    
    this->renderer->enableZTest(false);

    
    //2.Light Pass
    this->hdrBuffer->begin(false, false);
    this->lightPassShader->begin();
    
    
    uint32 light_idx = 0;
    this->lightPassShader->setParam("LightCount",
        static_cast<int>(this->renderer->renderLights.size()));
    for (auto& render_light : this->renderer->renderLights)
    {
        auto light = render_light.light;
        this->lightPassShader->setParam(
            String::fromFormat("lights[%d].Position", light_idx),
            render_light.transform->getPosition());
        this->lightPassShader->setParam(
            String::fromFormat("lights[%d].Color", light_idx),
            light->getColor());
        this->lightPassShader->setParam(
            String::fromFormat("lights[%d].Intensity", light_idx),
            light->getIntensity());
        this->lightPassShader->setParam(
            String::fromFormat("lights[%d].Linear", light_idx),
            light->getLinearAttenuation());
        this->lightPassShader->setParam(
            String::fromFormat("lights[%d].Quadratic", light_idx),
            light->getQuadraticAttenuation());
        ++light_idx;
    }
    
    auto camera = Kernel::instance()->lookup("/scene/camera");
    auto camera_t = camera->findComponent<TransformComponent>();
    auto camera_c = camera->findComponent<CameraComponent>();
    
    this->lightPassShader->setParam("ViewPos", camera_t->getPosition());
    this->lightPassShader->setParam("BrightThreshold", camera_c->getBrightThreshold());
    this->lightPassShader->setParam("AspectRatio", camera_c->getAspectRatio());
    this->lightPassShader->setParam("TanHalfFOV", Math::tan(Math::deg2rad(camera_c->getFov() / 2)));
    this->lightPassShader->setParam("ProjectionMatrix", camera_c->getProjectionMatrix());
    auto camera_view = camera_t->getWorldTransformMatrix();
    this->lightPassShader->setParam("InvViewMatrix", camera_view);
    
    
    this->lightPassShader->setParam("LightSpaceMatrix", this->lightSpaceMatrix);
    auto shadow_map_tex = this->renderer->createTexture("ShadowMap_Depth");
    shadow_map_tex->use(3);
    
    this->lightPassShader->commitParams();
    
    this->gBuffer->use();
    this->quadMesh->render();
    this->lightPassShader->end();
    this->hdrBuffer->end();
    
    
    
    //Bloom
    this->hdrBuffer->blitColorTarget(this->bloomBuffer[1], 1, 0);
    this->bloomBuffer[1]->getColorTexture(0)->use(0);
    bool horizontal = true, first_iteration = true;
    this->blurShader->begin();
    for (int i=0;i<2;++i)
    {
        this->bloomBuffer[i % 2]->begin(false, false);
        this->blurShader->setParam("horizontal", Vector2(horizontal?1:0, horizontal?0:1));
        this->blurShader->commitParams();
        if (first_iteration) first_iteration = false;
        else this->bloomBuffer[(i + 1) % 2]->getColorTexture(0)->use(0);
        this->quadMesh->render();
        this->bloomBuffer[i % 2]->end();
        horizontal = !horizontal;
    }
    this->blurShader->end();
    
    
    
    //DOF Blur
    this->hdrBuffer->blitColorTarget(this->dofBlurBuffer[1], 0, 0);
    this->dofBlurBuffer[1]->getColorTexture(0)->use(0);
    horizontal = true;
    first_iteration = true;
    this->blurShader->begin();
    for (int i=0;i<10;++i)
    {
        //이거 나중에 Blur Buffer 로 하는거 고민하자 (렌더타겟 재활용 해야함)
        this->dofBlurBuffer[i % 2]->begin(false, false);
        this->blurShader->setParam("horizontal", Vector2(horizontal?1:0, horizontal?0:1));
        this->blurShader->commitParams();
        if (first_iteration) first_iteration = false;
        else this->dofBlurBuffer[(i + 1) % 2]->getColorTexture(0)->use(0);
        this->quadMesh->render();
        this->dofBlurBuffer[i % 2]->end();
        horizontal = !horizontal;
    }
    this->blurShader->end();
    
    
    
    //SSAO
    this->renderer->setViewport(this->width, this->height);
    this->ssaoBuffer->begin(false, false);
    this->ssaoShader->begin();
    auto view = camera_t->getWorldTransformMatrix();
    view.inverse();
    this->ssaoShader->setParam("ProjectionMatrix",
        camera_c->getProjectionMatrix());
    this->ssaoShader->setParam("AspectRatio", camera_c->getAspectRatio());
    this->ssaoShader->setParam("TanHalfFOV", Math::tan(Math::deg2rad(camera_c->getFov() / 2)));
    this->gBuffer->getColorTexture(0)->use(0);
    this->noiseTexture->use(1);
    this->gBuffer->getDepthTexture()->use(2);
    this->ssaoShader->commitParams();
    this->quadMesh->render();
    this->ssaoShader->end();
    this->ssaoBuffer->end();
    
    
    //SSAO blur
    this->ssaoBlurBuffer->begin(false, false);
    this->ssaoBlurShader->begin();
    this->ssaoBuffer->use();
    this->quadMesh->render();
    this->ssaoBlurShader->end();
    this->ssaoBlurBuffer->end();
    
    
    
    //Combine final scene
    this->renderer->setViewport(width, height);
    //this->combineBuffer->begin(true, false);
    this->combineShader->begin();
    this->combineShader->setParam("Exposure", camera_c->getExposure());
    this->combineShader->setParam("Gamma", camera_c->getGamma());
    this->combineShader->setParam("DOFParam", camera_c->getDOF());
    this->combineShader->setParam("AspectRatio", camera_c->getAspectRatio());
    this->combineShader->setParam("TanHalfFOV", Math::tan(Math::deg2rad(camera_c->getFov() / 2)));
    this->combineShader->setParam("ProjectionMatrix", camera_c->getProjectionMatrix());
    this->combineShader->commitParams();
    this->hdrBuffer->getColorTexture(0)->use(0);
    this->bloomBuffer[1]->getColorTexture(0)->use(1);
    this->ssaoBlurBuffer->getColorTexture(0)->use(2);
    this->dofBlurBuffer[1]->getColorTexture(0)->use(3);
    this->gBuffer->getDepthTexture()->use(4);
    this->quadMesh->render();
    this->combineShader->end();
    //this->combineBuffer->end();
    
    return;
    


    /*this->renderer->setViewport(width, height);
    this->dofShader->begin();
    this->dofShader->setParam("DOFParam", camera_c->getDOF());
    this->dofShader->setParam("AspectRatio", camera_c->getAspectRatio());
    this->dofShader->setParam("TanHalfFOV", Math::tan(Math::deg2rad(camera_c->getFov() / 2)));
    this->dofShader->setParam("ProjectionMatrix", camera_c->getProjectionMatrix());
    this->dofShader->commitParams();
    this->combineBuffer->getColorTexture(0)->use(0);
    this->dofBlurBuffer[1]->getColorTexture(0)->use(1);
    this->gBuffer->getDepthTexture()->use(2);
    this->quadMesh->render();
    this->dofShader->end();*/
}


//-----------------------------------------------------------------------------
void DeferredShading::render_cameras()
{
    this->gBuffer->begin(false, true);
    
    auto shader = this->geometryPassShader.get();
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
            shader->setParam("WorldMatrix", proj_view_world);
            proj_view_world *= proj_view;
            shader->setParam("ProjViewWorldMatrix", proj_view_world);
            rc.render->updateShaderParams(shader);
            shader->commitParams();
            
            
            //Render Object
            rc.render->render();
        }
    }
    
    shader->end();
    this->gBuffer->end();
}


//-----------------------------------------------------------------------------
void DeferredShading::render_shadows()
{
    this->renderer->cullFace(CullFace::FRONT);

    for (auto& shadow : this->renderer->renderLights)
    {
        auto shadow_caster = shadow.shadowCaster;
        if (nullptr == shadow_caster) continue;
        
        shadow_caster->begin(shadow.transform->getWorldTransformMatrix());
        
        
        
        this->lightSpaceMatrix = shadow_caster->getLightSpaceMatrix();
        
        
        
        auto shadow_shader = shadow_caster->getShader();
        
        for (auto& rc : this->renderer->renderCommands)
        {
            auto obj_world_m = rc.transform->getWorldTransformMatrix();
            shadow_shader->setParam("WorldMatrix", obj_world_m);
            shadow_shader->commitParams();
            
            //Render Object
            rc.render->render();
        }
        
        shadow_caster->end();
    }
    
    this->renderer->cullFace(CullFace::BACK);
}


}

