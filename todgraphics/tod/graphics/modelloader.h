#pragma once
#include "tod/node.h"
namespace tod::graphics
{
class ModelLoaderPrivate;
class ModelLoader : public SingletonDerive<ModelLoader, Object>
{
public:
    ModelLoader();
    virtual~ModelLoader();
    
    Node* load(const String& uri);

private:
    ModelLoaderPrivate* inner;
};

}
