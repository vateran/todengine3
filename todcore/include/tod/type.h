#pragma once
#include <bitset>
#include <list>
#include <unordered_map>
#include "tod/platformdef.h"
#include "tod/string.h"
#include "tod/exception.h"
namespace tod
{
    
class RefCount;
class Object;
class Property;
class Method;

//-----------------------------------------------------------------------------
//!@brief Type이름에 관한 기능을 제공
//!@ingroup Reflection
class TypeNameDemangler
{
public:
    template <typename T>
    static String getFullName()
    {
        String name;
        
        #ifdef __GNUC__
        int status;
        name = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
        #else
        name = typeid(T).name();
        #endif
        
        return name;
    }
    
    
    template <typename T>
    static String getName()
    {
        String name { getFullName<T>() };
        return name.substr(name.rfind(":")+1, -1);
    }
};


//!@brief Object의 타입정보를 관리
//!@ingroup Reflection
class Type
{
public:
    typedef std::unordered_map<int, Property*> Properties;
    typedef std::list<Property*> PropertyOrder;
    typedef std::unordered_map<int, Method*> Methods;
    
public:
    Type(Type* base, const String& name);
    virtual~Type();
    
    void init();
    
    virtual Object* createObject()=0;
    virtual void bindProperty()=0;
    virtual void bindMethod()=0;
    virtual Object* getSingleton() { return nullptr; }
    
    void addProperty(Property* property);
    Property* findProperty(const String& prop_name);
    PropertyOrder& getPropertyOrder() { return this->propertyOrder; }
    PropertyOrder& getAllPropertyOrder() { return this->allPropertyOrder; }
    
    void addMethod(Method* method);
    Method* findMethod(const String& method_name);
    
    Type* getBase() { return this->base; }
    const String& getName() const { return this->name; }
    int getNameHash() const { return this->nameHash; }
    
private:
    String name;
    int nameHash;
    Type* base;
    Properties properties;
    PropertyOrder propertyOrder;
    PropertyOrder allPropertyOrder;
    Methods methods;
};


//-----------------------------------------------------------------------------
//!@brief 일반적인 Type의 구현 템플릿
//!@ingroup Reflection
template
<typename TYPE, typename BASE, bool ABSTRACT=std::is_abstract<TYPE>::value>
class ClassType : public Type
{
public:
    ClassType(Type* base, const String& name):
    Type(base, name)
    {
    }
    Object* createObject() override
    {
        return new TYPE();
    }
    void bindProperty() override
    {
        if (&TYPE::bindProperty != BASE::bindProperty)
            TYPE::bindProperty();
    }
    void bindMethod() override
    {
        if (&TYPE::bindMethod != BASE::bindMethod)
            TYPE::bindMethod();
    }
};
    

//-----------------------------------------------------------------------------
//!@brief BASE가 void 일경우 Type의 구현 템플릿
//!@ingroup Reflection
template <typename TYPE>
class ClassType<TYPE, void, false> : public Type
{
public:
    ClassType(Type* base, const String& name):
    Type(base, name)
    {
    }
    Object* createObject() override
    {
        return new TYPE();
    }
    void bindProperty() override
    {
        TYPE::bindProperty();
    }
    void bindMethod() override
    {
        TYPE::bindMethod();
    }
};
    

//!@brief abstract 클래스에 대한 Type의 구현 템플릿
//!@ingroup Reflection
template<typename TYPE, typename BASE>
class ClassType<TYPE, BASE, true>  : public Type
{
public:
    ClassType(Type* base, const String& name):
    Type(base, name)
    {
    }
    Object* createObject() override
    {
        return nullptr;
    }
    void bindProperty() override
    {
        if (&TYPE::bindProperty != BASE::bindProperty)
            TYPE::bindProperty();
    }
    void bindMethod() override
    {
        if (&TYPE::bindMethod != BASE::bindMethod)
            TYPE::bindMethod();
    }
};


//-----------------------------------------------------------------------------
//!@brief Singleton 클래스에 대한 Type의 구현 템플릿
//!@ingroup Reflection
template <typename TYPE, typename BASE>
class SingletonType : public ClassType<TYPE, BASE, false>
{
public:
    SingletonType(Type* base, const String& name):
    ClassType<TYPE, BASE, false>(base, name)
    {
    }
    TYPE* createObject() override
    {
        static TYPE* new_obj = nullptr;
        if (nullptr == new_obj)
        {
            new_obj = new TYPE();
        }
        return new_obj;
    }
    Object* getSingleton() override
    {
        return TYPE::instance();
    }
};


//-----------------------------------------------------------------------------
//!@brief 새로운 Object를 생성
//!@ingroup Core
template <typename TYPE>
TYPE* newInstance()
{
    auto new_obj = static_cast<TYPE*>(TYPE::get_type()->createObject());
    if (nullptr == new_obj)
        TOD_THROW_EXCEPTION("newInstance(%s)",
            TYPE::get_type()->getName().c_str());
    return new_obj;
}
    
#define REGISTER_TYPE(type) do { type :: get_type(); } while (false)

}
