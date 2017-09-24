#pragma once
#include <unordered_map>
#include "tod/type.h"
#include "tod/property.h"
#include "todeditor/foldwidget.h"
namespace tod::editor
{

class PropertySet : public FoldWidget
{
public:
    PropertySet(Object* object, Type* cur_type);
    void updateValues();
    void updateValue(Property* prop);
    
private:
    Object* object;
    std::unordered_map<Property*, std::function<void()>> updaters;
};

}
