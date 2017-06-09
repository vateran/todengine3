#include "tod/graphics/renderer.h"
namespace tod::graphics
{

bool Renderer::initialize()
{
    Matrix44 m;
    
    Kernel::instance()->init();
    std::list<String> paths;
	paths.push_back(".");
    paths.push_back("../../../..");
    auto asd=FileSystem::instance()->getCurrentWorkingDirectory();
    FileSystem::instance()->setSearchPaths(paths);
    
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    
    // uncomment these lines if on Apple OS X
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
    
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    
    
    auto node = Kernel::instance()->create("Node", "/node1");
    node->addComponent(newInstance<TransformComponent>());
    auto shader_component = newInstance<ShaderComponent>();
    shader_component->setVShaderFileName("BasicShader.glvs");
    shader_component->setFShaderFileName("BasicShader.glfs");
    node->addComponent(shader_component);
    node->addComponent(newInstance<MeshComponent>());
    
    
    while(!glfwWindowShouldClose(window))
    {
        this->mainloop();
        
        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }
    
    
    // close GL context and any other GLFW resources
    glfwTerminate();
    
    return true;
}
    
    
void Renderer::update_transform
(Node* current, const Matrix44& parent_transform, bool parent_transform_updated)
{
    if (!current->isVisible()) return;
    
    //TransformComponent로 WorldTransform 을 업데이트
    //Scene에 있는 모든 Node들은 TransformComponent가 있다고 가정한다
    auto tc = current->findComponent<TransformComponent>();
    if (nullptr == tc) return;
    
    //transform 이 변경되었다면 업데이트
    bool transform_dirty = parent_transform_updated
    || tc->isTransformDirty();
    if (transform_dirty)
        tc->updateWorldTransform(parent_transform);
    
    
    
    auto sc = current->findComponent<ShaderComponent>();
    if (nullptr != sc) sc->begin();
    
    
    
    auto rc = current->findComponent<RenderComponent>();
    if (nullptr != rc) rc->render();
    
    
    
    //자식 노드들의 transform 업데이트
    for (auto& child : current->getChildren())
    {
        this->update_transform(
                               child,
                               tc->getWorldTransformMatrix(),
                               transform_dirty);
    }
    
    
    if (nullptr != sc) sc->end();
}
    
}
