#pragma once
#include <map>
#include <unordered_map>
#include <functional>
#include "tod/object.h"
#include "tod/objref.h"
#include "tod/graphics/graphicsdef.h"
#include "tod/graphics/inputlayout.h"
namespace tod::graphics
{

class Shader;

class InputLayoutCache : public SingletonDerive<InputLayoutCache, Object>
{
public:
    typedef ObjRef<InputLayout> InputLayoutRef;
    typedef int32 NameHash;
    typedef std::unordered_map<NameHash, InputLayoutRef> InputLayouts;
    
    typedef String LayoutName;
    typedef std::map<LayoutName, InputLayout::LayoutElements*> LayoutConfigs;

public:
    InputLayoutCache();
    ~InputLayoutCache();

    bool createFromConfig(const String& uri);
    void destroy();

    InputLayout* getInputLayout(const String& technique_name);
    bool link(Shader* shader, const String& technique_name);

private:
    LayoutConfigs layoutConfigs;
    InputLayouts inputLayouts;
};

}