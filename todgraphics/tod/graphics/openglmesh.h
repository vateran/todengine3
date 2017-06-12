#pragma once
#include "tod/graphics/mesh.h"
namespace tod::graphics
{
    
class OpenGlMesh : public Derive<OpenGlMesh, Mesh>
{
public:
    OpenGlMesh():vbo(0), vao(0)
    {
        this->load();
    }
    virtual~OpenGlMesh()
    {
        this->unload();
    }
    
    void render() override
    {
        if (0 == this->vao) return;
        
        glBindVertexArray(this->vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, this->count, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glDrawArrays(GL_TRIANGLES, 0, this->count);
        
        glDisableVertexAttribArray(0);
    }
    
    void load() override
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
    
    void unload() override
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
    
}
