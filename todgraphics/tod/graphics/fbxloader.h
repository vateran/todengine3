#pragma once
#include "tod/node.h"
namespace tod::graphics
{

class FbxLoaderPrivate;
class FbxLoader : public SingletonDerive<FbxLoader, Object>
{
public:
    FbxLoader();
    virtual~FbxLoader();
    
    Node* load(const String& uri);

private:
    FbxLoaderPrivate* inner;
};
    
}
