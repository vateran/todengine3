#pragma once
#include "tod/graphics/renderer.h"
#include "tod/graphics/openglshader.h"
#include "tod/graphics/openglmesh.h"
namespace tod::graphics
{
    
class OpenGlRenderer : public Derive<OpenGlRenderer, Renderer>
{
public:
    bool initialize(
        void* window_handle,
        int width, int height,
        bool windowed) override;
    
    bool render(Camera* camera, Node* scene_root) override;
    
    Shader* createShader() override { return new OpenGlShader; }
    Mesh* createMesh() override { return new OpenGlMesh; }
};
    
}
