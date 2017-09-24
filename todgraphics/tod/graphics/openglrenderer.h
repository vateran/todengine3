#pragma once
#include "tod/graphics/renderer.h"
#include "tod/graphics/openglfunc.h"
namespace tod::graphics
{
    
class OpenGlRenderer : public Derive<OpenGlRenderer, Renderer>
{
public:
    bool initialize(
        void* opengl_func,
        int width, int height,
        bool windowed) override;
    
    void setViewport(int width, int height) override;
    void enableZTest(bool value) override;
    void enableAlphaTest(bool value) override;
    void cullFace(CullFace value) override;
    
    Shader* createShader() override;
    Mesh* createMesh(const String& name) override;
    Texture* createTexture(const String& name) override;
    RenderTarget* createRenderTarget(const String& name) override;
    Camera* createCamera() override;

};
    
}
