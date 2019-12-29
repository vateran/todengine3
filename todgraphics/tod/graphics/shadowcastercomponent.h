#pragma once
#include "tod/component.h"
#include "tod/graphics/matrix44.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/shadercache.h"
#include "tod/graphics/rendertarget.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/texture.h"
namespace tod::graphics
{

class RenderInterface;

class ShadowCasterComponent : public Derive<ShadowCasterComponent, Component>
{
public:
    ShadowCasterComponent()
    : renderInterface(nullptr)
    {
        /*this->shadowMapRT = this->renderInterface->createRenderTarget(S("ShadowMap"));
        this->shadowMapRT->create(1024, 1024);
        auto shadow_map = this->shadowMapRT->makeDepthTarget(Format::DEPTH16);
        shadow_map->setHorizontalWrap(TextureWrap::CLAMP_TO_EDGE);
        shadow_map->setVerticalWrap(TextureWrap::CLAMP_TO_EDGE);
        shadow_map->setBorderColor(ColorF(1,1,1,1));*/


        
        this->projMatrix.orthogonalRH(-20, 20, -20, 20, 0.1f, 100);
        //this->projMatrix.perspectiveRH(
        //        45, 1540.0f/1316, 0.1f, 10000.0f);
        
        /*this->shader = ShaderCache::instance()->
            getShader("EngineData/glsl/shadowmap.glvs",
                      "EngineData/glsl/shadowmap.glfs");*/
    }
    
    void begin(const Matrix44& t)
    {
        this->shadowMapRT->begin(false, true);
        uint32 passes = 0;
        this->shader->begin(0, passes);
        
        this->lightSpaceMatrix = t;
        this->lightSpaceMatrix.inverse();
        this->lightSpaceMatrix *= this->projMatrix;
        this->shader->setParam("LightSpaceMatrix", this->lightSpaceMatrix);
    }
    
    void end()
    {
        this->shader->end();
        this->shadowMapRT->end();
    }
    
    Shader* getShader()
    {
        return this->shader;
    }
    
    const Matrix44& getLightSpaceMatrix()
    {
        return this->lightSpaceMatrix;
    }
    

private:
    ObjRef<Shader> shader;
    ObjRef<RenderTarget> shadowMapRT;
    Matrix44 projMatrix;
    Matrix44 lightSpaceMatrix;
    RenderInterface* renderInterface;
};

}
