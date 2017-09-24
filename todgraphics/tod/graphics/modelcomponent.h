#pragma once
#include "tod/node.h"
#include "tod/objref.h"
namespace tod::graphics
{

class ModelComponent : public Derive<ModelComponent, Component>
{
public:
    ModelComponent();
    virtual~ModelComponent();
    
    void setUri(const String& uri);
    const String& getUri();
    
    static void bindProperty();
    
private:
    String uri;
    ObjRef<Node> meshNode;
};

}
