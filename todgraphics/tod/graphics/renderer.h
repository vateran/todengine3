#pragma once
#include "tod/coreforward.h"
#include "tod/filesystem.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/matrix44.h"

#ifdef PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
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


class Shader
{
public:
    Shader():vshader(0), fshader(0), program(0)
    {
    }
    ~Shader()
    {
        this->unload();
    }
    
    virtual bool begin()
    {
        if (0 == this->program) return false;
        glUseProgram(this->program);
        return true;
    }
    
    virtual void end()
    {
        glUseProgram(0);
    }
    
    virtual bool load(const String& vshader_fname, const String& fshader_fname)
    {
        this->unload();
        
        //Load Vertext Shader
        if (!this->load_shader(
            vshader_fname,
            this->vshader,
            GL_VERTEX_SHADER)) return false;
        
        //Load Fragment Shader
        if (!this->load_shader(
            fshader_fname,
            this->fshader,
            GL_FRAGMENT_SHADER)) return false;
        
        this->program = glCreateProgram();
        glAttachShader(this->program, this->vshader);
        glAttachShader(this->program, this->fshader);
        glLinkProgram(this->program);
        if (!this->check_program(this->program))
        {
            this->unload();
            return false;
        }

        return true;
    }
    
    virtual void unload()
    {
        if (0 != this->fshader)
        {
            glDetachShader(this->program, this->fshader);
            glDeleteShader(this->fshader);
        }
        if (0 != this->vshader)
        {
            glDetachShader(this->program, this->vshader);
            glDeleteShader(this->vshader);
        }
        if (0 != this->program)
            glDeleteProgram(this->program);
        this->fshader = 0;
        this->vshader = 0;
        this->program = 0;
    }
    
private:
    bool load_shader(const String& file_name, GLuint& shader, GLenum shader_type)
    {
        if (!FileSystem::instance()->load(
            file_name, [this, &shader, shader_type](FileSystem::Data& data)
            {
                const char* shader_data = &data[0];
                shader = glCreateShader(shader_type);
                glShaderSource(shader, 1, &shader_data, nullptr);
                glCompileShader(shader);
                if (!this->check_shader(shader))
                {
                    this->unload();
                    return false;
                }
                return true;
            }, FileSystem::LoadOption().string())) return false;
        return true;
    }
    bool check_shader(GLuint shader)
    {
        GLint state;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
        if (GL_FALSE == state)
        {
            int infolog_len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infolog_len);
            if (infolog_len > 1)
            {
                int chars = 0;
                std::string error_log;
                error_log.resize(infolog_len);
                glGetShaderInfoLog(shader, infolog_len, &chars, &error_log[0]);
                TOD_THROW_EXCEPTION(error_log.c_str());
            }
            return false;
        }
        return true;
    }
    bool check_program(GLuint program)
    {
        GLint state;
        glGetProgramiv(program, GL_LINK_STATUS, &state);
        if (GL_FALSE == state)
        {
            int infolog_len = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolog_len);
            if (infolog_len > 1)
            {
                int chars = 0;
                std::string error_log;
                error_log.resize(infolog_len);
                glGetProgramInfoLog(program, infolog_len, &chars, &error_log[0]);
                TOD_THROW_EXCEPTION(error_log.c_str());
            }
            return false;
        }
        return true;
    }
    
private:
    GLuint vshader;
    GLuint fshader;
    GLuint program;
};

class Light
{
public:
};

class Mesh
{
public:
    Mesh():vbo(0), vao(0)
    {
        this->load();
    }
    ~Mesh()
    {
        this->unload();
    }
    
    virtual void render()
    {
        if (0 == this->vao) return;
        
        glBindVertexArray(this->vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, this->count, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glDrawArrays(GL_TRIANGLES, 0, this->count);
        
        glDisableVertexAttribArray(0);
        //glBindVertexArray(0);
    }
    
    virtual void load()
    {
        float points[] = {
            0.0f,  0.5f,  0.0f,
            0.5f, -0.5f,  0.0f,
            -0.5f, -0.5f,  0.0f
        };
        this->count = 3;
        
        glGenVertexArrays(1, &this->vao);
        glBindVertexArray(this->vao);
        
        glGenBuffers(1, &this->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, this->count * 3 * sizeof(float), points, GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
    }
    
    virtual void unload()
    {
        if (0 != this->vbo) glDeleteBuffers(1, &this->vbo);
        this->vbo = 0;
        
        if (0 != this->vao) glDeleteVertexArrays(1, &this->vao);
        this->vao = 0;
    }
    
private:
    GLuint vbo;
    GLuint vao;
    GLsizei count;
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
    
class MeshComponent : public Derive<MeshComponent, RenderComponent>
{
public:
    MeshComponent():
    mesh(nullptr)
    {
        this->mesh = new Mesh();
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
};
    
class ShaderComponent;

class Renderer : public Derive<Renderer, Node>
{
public:
    Camera camera;
    
    virtual bool initialize(void* window_handle, int width, int height, bool windowed);
    
    virtual void mainloop()
    {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        this->render(&this->camera, Kernel::instance()->lookup("/node1"));
    }
    
    virtual bool render(Camera* camera, Node* scene_root)
    {
        Matrix44 t;
        t.identity();
        this->update_transform(scene_root, t, false);
        
        return true;
    }
    
private:
    void update_transform(Node* current, const Matrix44& parent_transform,
                          bool parent_transform_updated);
    
private:
    struct RenderCommand
    {
    public:
        Matrix44 worldTransform;
        TransformComponent* tansform;
        ShaderComponent* shader;
        RenderComponent* render;
    };
    typedef std::shared_ptr<RenderCommand> RenderCommandPtr;
    typedef std::vector<RenderCommandPtr> RenderCommands;
    RenderCommands renderCommands;
};
    
class ShaderComponent : public Derive<ShaderComponent, Component>
{
public:
    ShaderComponent():
    shader(nullptr)
    {
        this->shader = new Shader();
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
};
    
}
