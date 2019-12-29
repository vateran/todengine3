#pragma once
namespace tod::graphics
{

class Viewport;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class InputLayout;
class Texture;
class Shader;

class RenderInterface
{
public:
    virtual~RenderInterface() {}

    virtual bool create() = 0;
    virtual void destroy() = 0;

    virtual void beginScene() = 0;
    virtual void endScene() = 0;

    virtual Viewport* createViewport() = 0;
    virtual VertexBuffer* createVertexBuffer() = 0;
    virtual IndexBuffer* createIndexBuffer() = 0;
    virtual ConstantBuffer* createConstantBuffer(const String& name) = 0;
    virtual InputLayout* createInputLayout() = 0;
    virtual Texture* createTexture() = 0;
    virtual Shader* createShader() = 0;

    virtual void drawPrimitive(uint32 count) = 0;

    virtual void setWireframeMode(bool enable) = 0;
};

}
