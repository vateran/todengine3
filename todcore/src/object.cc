#include "tod/precompiled.h"
#include "tod/uuid.h"
#include "tod/method.h"
#include "tod/object.h"
namespace tod
{
    
//-----------------------------------------------------------------------------
Object::Object()
: refCount(0)
, dynamicProperties(nullptr)
{
}


//-----------------------------------------------------------------------------
Object::~Object()
{
    TOD_SAFE_DELETE(this->dynamicProperties);
}
    

//-----------------------------------------------------------------------------
void Object::retain()
{
    ++this->refCount;
}
    

//-----------------------------------------------------------------------------
int32 Object::release()
{
    TOD_ASSERT(0 <= this->refCount, "이미 refCount <= 0 입니다");
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
    auto prop = this->getType()->findProperty(prop_name);
    if (nullptr != prop) return prop;
    return this->findDynamicProperty(prop_name);
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
Property* Object::findDynamicProperty(const String& prop_name)
{
    if (nullptr == this->dynamicProperties) TOD_RETURN_TRACE(nullptr);
    
    auto i = this->dynamicProperties->find(prop_name.hash());
    if (this->dynamicProperties->end() == i) TOD_RETURN_TRACE(nullptr);
    return i->second;
}


//-----------------------------------------------------------------------------
void Object::removeDynamicProperty(const String& prop_name)
{
    if (nullptr == this->dynamicProperties) return;
    
    this->dynamicProperties->erase(prop_name.hash());
    if (this->dynamicProperties->empty())
        TOD_SAFE_DELETE(this->dynamicProperties);
}


//-----------------------------------------------------------------------------
Object::DynamicProperties* Object::getDynamicProperties()
{
    return this->dynamicProperties;
}
    
    
//-----------------------------------------------------------------------------
void Object::bindProperty()
{
    BIND_PROPERTY_READONLY(int, "ref_count", "Object's reference counter",
                           getRefCount, 0, PropertyAttr::READ);
}
    
}
