#pragma once
#include "tod/coreforward.h"
#include "tod/filesystem.h"
#include "tod/rect.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/vector4.h"
#include "tod/graphics/matrix44.h"
#include "tod/graphics/transform44.h"
namespace tod::graphics
{

class Shader;
class Texture;
class Mesh;
class RenderTarget;
class Camera;
class TransformComponent;
class RenderComponent;
class ShaderComponent;
class CameraComponent;
class LightComponent;
class ShadowCasterComponent;
class Shading;
class DeferredShading;

enum class CullFace
{
    FRONT,
    BACK,
};

class Renderer : public Derive<Renderer, Node>
{
public:
    typedef std::unordered_map<int, ObjRef<Texture>> NamedTextures;
    typedef std::unordered_map<int, ObjRef<Mesh>> NamedMeshes;
    
public:
    Renderer();
    virtual~Renderer();

    virtual bool initialize(void* window_handle, int width, int height, bool windowed);
    virtual bool render(Node* scene_root);
    
    virtual void setViewport(int width, int height) {}
    virtual void drawTexture(Texture* texture);
    virtual void enableZTest(bool value) {}
    virtual void enableAlphaTest(bool value) {}
    virtual void cullFace(CullFace value) {}
    
    virtual Shader* createShader()=0;
    virtual Mesh* createMesh(const String& name=S(""))=0;
    virtual Texture* createTexture(const String& name=S(""))=0;
    virtual RenderTarget* createRenderTarget(const String& name)=0;
    virtual Camera* createCamera()=0;
    
    NamedTextures& getNamedTextures() { return this->namedTextures; }
    
    void setSSAOKernelSize(int value);
    int getSSAOKernelSize();
    void setSSAOSampleRadius(float value);
    float getSSAOSampleRadius();
    void setSSAOBias(float value);
    float getSSAOBias();
    void setSSAOPower(float value);
    float getSSAOPower();
    
    static void bindProperty();
    
public:
    static float ScreenScale() { return 2.0f; }
    
protected:
    void update_transform(Node* current, const Matrix44& parent_transform,
                          bool parent_transform_updated);
    void render_cameras();
    
protected:
    //RenderCommand list
    struct RenderCommand
    {
        Node* node;
        TransformComponent* transform;
        ShaderComponent* shader;
        RenderComponent* render;
    };
    typedef std::list<RenderCommand> RenderCommands;
    RenderCommands renderCommands;
    
    
    //Camera list
    struct RenderCamera
    {
        TransformComponent* transform;
        ShaderComponent* shader;
        CameraComponent* camera;
    };
    typedef std::list<RenderCamera> RenderCameras;
    RenderCameras renderCameras;
    
    
    //Light list
    struct RenderLight
    {
        TransformComponent* transform;
        ShaderComponent* shader;
        LightComponent* light;
        ShadowCasterComponent* shadowCaster;
    };
    typedef std::list<RenderLight> RenderLights;
    RenderLights renderLights;
    
    
    //ShaderStack
    std::stack<ShaderComponent*> shaderStack;
    
    
    //Named Resources
    NamedTextures namedTextures;
    NamedMeshes namedMeshes;
    
    
    Shading* shading;
    
    
    //Deferred Rendering stuff
    friend class DeferredShading;
    DeferredShading* deferredShading;
    
    
    //for Draw Texture
    ObjRef<Mesh> quadMesh;
    ObjRef<Shader> drawTextureShader;
};
    
}
