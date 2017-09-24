#pragma once
#include "tod/objref.h"
#include "tod/graphics/shading.h"
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

class Shader;
class RenderTarget;
class Mesh;
class Texture;
class DeferredShading : public Shading
{
public:
    DeferredShading();
    friend class Renderer;
    
    void init(Renderer* renderer, int width, int height) override;
    void render() override;
    void render_cameras();
    void render_shadows();

private:
    int width;
    int height;
    Renderer* renderer;
    ObjRef<Shader> geometryPassShader;
    ObjRef<Shader> lightPassShader;
    ObjRef<Shader> blurShader;
    ObjRef<Shader> ssaoShader;
    ObjRef<Shader> ssaoBlurShader;
    ObjRef<Shader> combineShader;
    ObjRef<Shader> dofBlurShader;
    ObjRef<Shader> dofShader;
    ObjRef<RenderTarget> gBuffer;
    ObjRef<RenderTarget> hdrBuffer;
    ObjRef<RenderTarget> bloomBuffer[2];
    ObjRef<RenderTarget> ssaoBuffer;
    ObjRef<RenderTarget> ssaoBlurBuffer;
    ObjRef<RenderTarget> dofBlurBuffer[2];
    ObjRef<RenderTarget> combineBuffer;
    ObjRef<Texture> noiseTexture;
    ObjRef<Mesh> quadMesh;
    
    
    Matrix44 lightSpaceMatrix;
    
    
    int ssaoKernelSize;
    float ssaoSampleRadius;
    float ssaoBias;
    float ssaoPower;

};

}
