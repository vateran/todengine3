#pragma once
#include "tod/graphics/renderer.h"
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
};
    
}
