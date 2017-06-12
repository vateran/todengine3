#ifdef PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif
#include "tod/graphics/openglrenderer.h"
#include "tod/filesystem.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

//Library link
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3dll.lib")

namespace tod::graphics
{
    
//-----------------------------------------------------------------------------
bool OpenGlRenderer::initialize
(void* window_handle, int width, int height, bool windowed)
{
    glewExperimental = GL_TRUE;
    glewInit();
    
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    auto node = Kernel::instance()->create("Node", "/node1");
    node->addComponent(newInstance<TransformComponent>());
    auto shader_component = newInstance<ShaderComponent>();
    shader_component->setVShaderFileName("BasicShader.glvs");
    shader_component->setFShaderFileName("BasicShader.glfs");
    node->addComponent(shader_component);
    node->addComponent(newInstance<MeshComponent>());
    
    return true;
}

    
//-----------------------------------------------------------------------------
bool OpenGlRenderer::render(Camera* camera, Node* scene_root)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Matrix44 t;
    t.identity();
    this->update_transform(scene_root, t, false);
    
    return true;
}
    
}
