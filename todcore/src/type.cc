#include "tod/type.h"
#include "tod/kernel.h"
#include "tod/property.h"
#include "tod/method.h"
#include <stack>
namespace tod
{
 
//-----------------------------------------------------------------------------
Type::Type(Type* base, const String& name):
 base(base)
,name(name)
,nameHash(this->name.hash())
{
}

    
//-----------------------------------------------------------------------------
Type::~Type()
{
    for (auto& p : this->properties) delete p.second;
    for (auto& m : this->methods) delete m.second;
}

    
//-----------------------------------------------------------------------------
void Type::init()
{
    //Property 와 Method를 등록
    this->bindProperty();
    this->bindMethod();
    
    
    //PropertyOrder를 구성한다(모든 Base->Current Type의 Properties)
    std::stack<Type*> types;
    Type* cur = this->base;
    while (cur)
    {
        types.push(cur);
        cur = cur->base;
    }
    while (!types.empty())
    {
        cur = types.top();
        this->allPropertyOrder.insert(this->allPropertyOrder.begin(),
                                      cur->propertyOrder.begin(),
                                      cur->propertyOrder.end());
        types.pop();
    }
    
    
    //Kernel 에 등록
    Kernel::instance()->addType(this);
}
    
    
//-----------------------------------------------------------------------------
void Type::addProperty(Property* property)
{
    //addProperty 는 시스템이 가동될때 주로 호출된다.
    auto name_hash = property->getName().hash();
    if (this->properties.end() != this->properties.find(name_hash))
        TOD_THROW_EXCEPTION("alreay exist propery name(%s::%s)",
                            this->getName().c_str(),
                            property->getName().c_str());
    this->properties.insert(std::make_pair(name_hash, property));
    this->propertyOrder.push_back(property);
    this->allPropertyOrder.push_back(property);
}
    

//-----------------------------------------------------------------------------
Property* Type::findProperty(const String& prop_name)
{
    //Derive -> Base 로 이동하면서 prop_name 을 갖는 Property 를 찾는다
    auto prop_name_hash = prop_name.hash();
    auto cur_type = this;
    while (cur_type)
    {
        auto i = cur_type->properties.find(prop_name_hash);
        if (cur_type->properties.end() != i) return i->second;
        cur_type = cur_type->base;
    }
    return nullptr;
}
    

//-----------------------------------------------------------------------------
void Type::addMethod(Method* method)
{
    //addMethod 는 시스템이 가동될때 주로 호출된다.
    auto name_hash = method->getName().hash();
    if (this->methods.end() != this->methods.find(name_hash))
        TOD_THROW_EXCEPTION("alreay exist method name(%s::%s)",
                            this->getName().c_str(),
                            method->getName().c_str());
    this->methods.insert(std::make_pair(name_hash, method));
}
    
    
//-----------------------------------------------------------------------------
Method* Type::findMethod(const String& method_name)
{
    //Derive -> Base 로 이동하면서 method_name 을 갖는 Method 를 찾는다
    auto method_name_hash = method_name.hash();
    auto cur_type = this;
    while (cur_type)
    {
        auto i = cur_type->methods.find(method_name_hash);
        if (cur_type->methods.end() != i) return i->second;
        cur_type = cur_type->base;
    }
    return nullptr;
}
    
    
}
