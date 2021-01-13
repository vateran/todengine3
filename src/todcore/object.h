#pragma once
#include "tod/any.h"
#include "tod/derive.h"
#include "tod/property.h"
#include "tod/indexedpool.h"
namespace tod
{

//!@ingroup Core
//!@brief TodEngine에서 사용하는 모든 객체의 최상위 클래스
//!- Refence Count 기반의 수명관리
//!- 객체의 Type @ref Reflection 제공
//!- Dynamic Property 제공
class Object : public Derive<Object, void>
{
public:
    typedef int32 NameHash;
    typedef std::unordered_map<NameHash, Property*> DynamicProperties;

public:
    Object();
    virtual~Object();
    
    void retain();
    virtual int32 release();
    inline int32 getRefCount() { return this->refCount; }
    
    template <typename T>
    bool isKindOf() const;
    
    Property* findProperty(const String& prop_name);
    void resetAllProperties();
    Method* findMethod(const String& method_name);
    
    template <typename T>
    Property* addDynamicProperty(const String& prop_name, const std::any& value);
    Property* findDynamicProperty(const String& prop_name);
    void removeDynamicProperty(const String& prop_name);
    DynamicProperties* getDynamicProperties();
        
    static void bindProperty();
    static void bindMethod() {}
    
protected:
    int32 refCount;
    DynamicProperties* dynamicProperties;
};



//-----------------------------------------------------------------------------
template <typename T>
Property* Object::addDynamicProperty
(const String& prop_name, const std::any& value)
{
    if (nullptr == this->dynamicProperties)
    {
        this->dynamicProperties = new DynamicProperties();
    }
    
    auto dyn_prop = new DynamicPropertyBind<SelfType, T>(prop_name);
    dyn_prop->set(value);
    dyn_prop->setDefaultValue(value);
    this->dynamicProperties->insert(std::make_pair(prop_name.hash(), dyn_prop));
    return dyn_prop;
}


//-----------------------------------------------------------------------------
template <typename T>
bool Object::isKindOf() const
{
    return getType()->isKindOf<T>();
}


}
