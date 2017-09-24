#include "tod/graphics/openglmesh.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/fbxloader.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
OpenGlMesh::OpenGlMesh(const String& name):
  vao(0)
, vbo(0)
, ebo(0)
, drawMode(GL_TRIANGLES)
, lockData(nullptr)
{
    this->name = name;
    this->load();
}


//-----------------------------------------------------------------------------
OpenGlMesh::~OpenGlMesh()
{
    this->unload();
}


//-----------------------------------------------------------------------------
void OpenGlMesh::render()
{
    if (0 == this->vao) return;
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    
    OpenGl::funcs->glBindVertexArray(this->vao);
    
    if (0 != this->ebo)
    {
        OpenGl::funcs->glDrawElements(
            this->drawMode, static_cast<GLsizei>(this->indices.size()),
            GL_UNSIGNED_INT, 0);
    }
    else
    {
        OpenGl::funcs->glDrawArrays(this->drawMode, 0, this->vertexCount);
    }
    
    OpenGl::funcs->glBindVertexArray(0);
}


//-----------------------------------------------------------------------------
void OpenGlMesh::setDrawType(DrawType value)
{
    BaseType::setDrawType(value);
    
    switch (value)
    {
    case DRAWTYPE_TRIANGLES:
        this->drawMode = GL_TRIANGLES;
        break;
    case DRAWTYPE_TRIANGLE_STRIP:
        this->drawMode = GL_TRIANGLE_STRIP;
        break;
    }
}


//-----------------------------------------------------------------------------
void OpenGlMesh::setVertexDataPtr(VertexStructInfo& info, int index, void* data)
{
    auto ptr = this->lockData + (this->stride * index) + info.startOffset;
    memcpy(ptr, data, info.metaInfo->dataSize);
}


//-----------------------------------------------------------------------------
void* OpenGlMesh::lock(int vertex_count, int offset)
{
    this->vertexCount = vertex_count;
    OpenGl::funcs->glBindVertexArray(this->vao);
    if (0 == this->vbo)
    {
        OpenGl::funcs->glGenBuffers(1, &this->vbo);
        OpenGl::funcs->glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        OpenGl::funcs->glBufferData(
            GL_ARRAY_BUFFER, vertex_count * this->stride,
            nullptr, GL_STATIC_DRAW);
        
        for (auto& info : this->vertexStructInfo)
            info.writePos = 0;
    }
    else
    {
        OpenGl::funcs->glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    }
    
    this->lockData = static_cast<uint8*>(OpenGl::funcs->glMapBufferRange(
        GL_ARRAY_BUFFER, offset, vertex_count * this->stride, GL_MAP_READ_BIT));
    return this->lockData;
}


//-----------------------------------------------------------------------------
void OpenGlMesh::unlock()
{
    OpenGl::funcs->glBindVertexArray(this->vao);
    OpenGl::funcs->glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    OpenGl::funcs->glUnmapBuffer(GL_ARRAY_BUFFER);
    this->lockData = nullptr;
    
    for (int i=0;i<this->vertexStructInfo.size();++i)
    {
        auto& info = this->vertexStructInfo[i];
        if (!info.enable) continue;
        OpenGl::funcs->glVertexAttribPointer(
            i, info.metaInfo->factorCount, GL_FLOAT, GL_FALSE,
            this->stride, reinterpret_cast<void*>(info.startOffset));
        OpenGl::funcs->glEnableVertexAttribArray(i);
    }
    
    if (!this->indices.empty())
    {
        if (0 == this->ebo)
        {
            OpenGl::funcs->glGenBuffers(1, &this->ebo);
            OpenGl::funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
        }
        OpenGl::funcs->glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint32),
            &this->indices[0], GL_STATIC_DRAW);
    }
    else
    {
        if (0 != this->ebo)
        {
            OpenGl::funcs->glDeleteBuffers(1, &this->ebo);
            this->ebo = 0;
        }
    }
}


//-----------------------------------------------------------------------------
void OpenGlMesh::load()
{
    OpenGl::funcs->glGenVertexArrays(1, &this->vao);
}


//-----------------------------------------------------------------------------
void OpenGlMesh::unload()
{
    if (0 != this->ebo)
        OpenGl::funcs->glDeleteBuffers(1, &this->ebo);
    this->ebo = 0;

    if (0 != this->vbo)
        OpenGl::funcs->glDeleteBuffers(1, &this->vbo);
    this->vbo = 0;
    
    if (0 != this->vao)
        OpenGl::funcs->glDeleteVertexArrays(1, &this->vao);
    this->vao = 0;
}

}
