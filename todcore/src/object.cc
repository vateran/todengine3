#include "tod/object.h"
namespace tod
{
    
//-----------------------------------------------------------------------------
Object::Object():
refCount(0)
{
}
    

//-----------------------------------------------------------------------------
void Object::retain()
{
    ++this->refCount;
}
    

//-----------------------------------------------------------------------------
int Object::release()
{
    if (--this->refCount <= 0)
    {
        delete this;
        return 0;
    }
    return this->refCount;
}

    
//-----------------------------------------------------------------------------
Property* Object::findProperty(const String& prop_name)
{
    return this->getType()->findProperty(prop_name);
}
    

//-----------------------------------------------------------------------------
void Object::resetAllProperties()
{
    for (auto& p : this->getType()->getAllPropertyOrder())
    {
        p->setDefaultValue(this);
    }
}

    
//-----------------------------------------------------------------------------
Method* Object::findMethod(const String& method_name)
{
    return this->getType()->findMethod(method_name);
}
    
    
//-----------------------------------------------------------------------------
void Object::bindProperty()
{
    BIND_PROPERTY_READONLY(int, "ref_count", "Object's reference counter",
                           getRefCount, 0, PropertyAttr::READ);
}
    
}
