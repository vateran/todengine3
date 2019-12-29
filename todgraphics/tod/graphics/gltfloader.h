#pragma once
#include "tod/object.h"
namespace tod::graphics
{

class Mesh;
class GLTFLoader : public SingletonDerive<GLTFLoader, Object>
{
public:
    GLTFLoader();
    virtual~GLTFLoader();

    bool importMesh(const String& uri, OUT Mesh** mesh);

private:
    class GLTFLoaderPrivate;
    std::shared_ptr<GLTFLoaderPrivate> inner;
};

}