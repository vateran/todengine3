#pragma once
#include "tod/graphics/openglfunc.h"
#include "tod/graphics/mesh.h"
namespace tod::graphics
{
 
class OpenGlMesh : public Derive<OpenGlMesh, Mesh>
{
public:
    OpenGlMesh(const String& name=S(""));
    virtual~OpenGlMesh();
    
    void render() override;
    void setDrawType(DrawType value) override;
    void setVertexDataPtr(
        VertexStructInfo& info, int index, void* data) override;
    void* lock(int vertex_count, int offset=0) override;
    void unlock() override;
    void load() override;
    void unload() override;
    
private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLenum drawMode;
    uint8* lockData;
};
    
}
