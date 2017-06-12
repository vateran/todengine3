#pragma once
#include "tod/coreforward.h"
#include "tod/filesystem.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/matrix44.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/mesh.h"
namespace tod::graphics
{
    
class Renderer;
typedef int64 ResourceId;

template <typename T>
class RectBase
{
public:
    T x, y, w, h;
};
typedef RectBase<float> Rect;
    
    
class Camera
{
public:
    
private:
    bool clearColor;
    bool clearDepth;
    bool clearStencil;
    //float near;
    //float far;
    Rect viewportRect;
};



class Light
{
public:
};
    
class BoundingVolume
{
public:
    virtual bool isCulled(Camera* camera)=0;
    virtual void expand(BoundingVolume* other)=0;
};
    
class SphereBoundingVolume : public BoundingVolume
{
public:
    virtual bool isCulled(Camera* camera)
    {
        return false;
    }
    virtual void expand(BoundingVolume* other)
    {
    }
};
    
class TransformComponent : public Derive<TransformComponent, Component>
{
public:
    enum
    {
        TRANSFORM_DIRTY,
        
        MAX,
    };
public:
    TransformComponent()
    {
        this->localTransform.identity();
        this->flags[TRANSFORM_DIRTY] = true;
    }
    void updateWorldTransform(const Matrix44& parent_transform)
    {
        //TODO : 이렇게 곱하는거랑 곱하면서 복사하는 거랑 어느쪽이 빠른지 대결해보자
        this->worldTransform = this->localTransform;
        this->worldTransform *= parent_transform;
        this->flags[TRANSFORM_DIRTY] = false;
    }
    void setTranslation(const Vector3& t)
    {
        this->localTransform.setTranslation(t);
        this->flags[TRANSFORM_DIRTY] = true;
    }
    bool isTransformDirty() const { return this->flags[TRANSFORM_DIRTY]; }
    const Matrix44& getLocalTransformMatrix() const
    { return this->localTransform; }
    //TODO : 아직 이번 프레임이 렌더링 되지 않았을 경우 worldTransform 이 업데이트 안된 경우가 있다
    //아직 렌더링 되지 않았을때도 올바른 WorldTransform을 얻도록 해야함
    const Matrix44& getWorldTransformMatrix() const
    { return this->worldTransform; }
    
private:
    Matrix44 localTransform;
    Matrix44 worldTransform;
    BoundingVolume* boundingVolume;
    std::bitset<MAX> flags;
};
    
class RenderComponent : public Derive<RenderComponent, Component>
{
public:
    virtual void render()=0;
};
    


class ShaderComponent;

class Renderer : public Derive<Renderer, Node>
{
public:
    Camera camera;
    
    virtual bool initialize(void* window_handle, int width, int height, bool windowed) { return false; }
    virtual bool render(Camera* camera, Node* scene_root) { return false; }
    virtual Shader* createShader()=0;
    virtual Mesh* createMesh()=0;
    
protected:
    void update_transform(Node* current, const Matrix44& parent_transform,
                          bool parent_transform_updated);
};
    
class ShaderComponent : public Derive<ShaderComponent, Component>
{
public:
    ShaderComponent():
    shader(nullptr),
    renderer("/sys/renderer")
    {
        this->shader = this->renderer->createShader();
    }
    virtual~ShaderComponent()
    {
        SAFE_DELETE(this->shader);
    }
    virtual void begin()
    {
        if (nullptr == this->shader) return;
        shader->begin();
    }
    virtual void end()
    {
        if (nullptr == this->shader) return;
        shader->end();
    }
    void setVShaderFileName(const String& fname)
    {
        this->vshaderFileName = fname;
        
        this->load();
    }
    const String& getVShaderFileName()
    {
        return this->vshaderFileName;
    }
    void setFShaderFileName(const String& fname)
    {
        this->fshaderFileName = fname;
        
        this->load();
    }
    const String& getFShaderFileName()
    {
        return this->fshaderFileName;
    }
    
    static void bindProperty()
    {
        BIND_PROPERTY(const String&, "vshader_fname", "",
            setVShaderFileName, getVShaderFileName, "", PropertyAttr::DEFAULT);
        BIND_PROPERTY(const String&, "fshader_fname", "",
            setFShaderFileName, getFShaderFileName, "", PropertyAttr::DEFAULT);
    }
    
private:
    void load()
    {
        if (this->vshaderFileName.empty() || this->fshaderFileName.empty())
            return;
        this->shader->load(this->vshaderFileName, this->fshaderFileName);
    }
    
private:
    String vshaderFileName;
    String fshaderFileName;
    Shader* shader;
    ObjRef<Renderer> renderer;
};
    
class MeshComponent : public Derive<MeshComponent, RenderComponent>
{
public:
    MeshComponent():
    mesh(nullptr),
    renderer("/sys/renderer")
    {
        this->mesh = this->renderer->createMesh();
    }
    virtual~MeshComponent()
    {
        SAFE_DELETE(this->mesh);
    }
    
    void render() override
    {
        if (nullptr == this->mesh) return;
        this->mesh->render();
    }
    
private:
    Mesh* mesh;
    ObjRef<Renderer> renderer;
};

    
}
