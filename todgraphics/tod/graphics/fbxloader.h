#pragma once
#include "tod/node.h"
namespace tod::graphics
{

class Mesh;
class FbxLoaderPrivate;
class FbxLoader : public SingletonDerive<FbxLoader, Object>
{
public:
    FbxLoader();
    virtual~FbxLoader();
    
    bool importMesh(const String& uri, Mesh** mesh);

private:
    FbxLoaderPrivate* inner;
};
    
}
