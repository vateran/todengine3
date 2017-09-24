#include "tod/graphics/primitivemesh.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/mesh.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
PrimitiveMesh::PrimitiveMesh()
{
    ObjRef<Renderer> renderer(S("/sys/renderer"));

    auto quad_mesh = renderer->createMesh(S("quad"));
    quad_mesh->setupVertexStruct(VertexDataOption()
        .add(VertexDataType::POSITION)
        .add(VertexDataType::TEXCOORD));
    auto ptr = quad_mesh->lock(4);
    float quad_vertices[] =
    {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    memcpy(ptr, quad_vertices, sizeof(quad_vertices));
    quad_mesh->unlock();
    quad_mesh->setDrawType(Mesh::DRAWTYPE_TRIANGLE_STRIP);
}

}
