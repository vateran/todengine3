#pragma once
#include <unordered_map>
#include "tod/exception.h"
#include "tod/staticstring.h"
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
    static const String& getFullName()
    {
        static String name;
        
        #ifdef __GNUC__
        int32 status;
        name = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
        #else
        name = typeid(T).name();
        #endif
        
        return name;
    }
    
    
    template <typename T>
    static const String& getName()
    {
        static String name;
        if (name.empty())
        {
            name = getFullName<T>();
            name = name.substr(name.rfind(S(":")) + 1, -1);
            #if defined(TOD_PLATFORM_WINDOWS)
            name = name.substr(name.find(S(" ")) +1, -1);
            #endif
        }
        return name;
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
    typedef std::list<Type*> DerivedTypes;
    
public:
    Type(Type* base, const StaticString& name);
    virtual~Type();
    
    void init();
    
    virtual Object* createObject()=0;
    virtual void bindProperty()=0;
    virtual void bindMethod()=0;
    
    void addProperty(Property* property);
    Property* findProperty(const String& prop_name);
    PropertyOrder& getPropertyOrder() { return this->propertyOrder; }
    PropertyOrder& getAllPropertyOrder() { return this->allPropertyOrder; }
    
    void addMethod(Method* method);
    Method* findMethod(const String& method_name);
    
    void setAbstract(bool value) { this->abstract = value; }
    bool isAbstract() { return this->abstract; }
    Type* getBase() { return this->base; }
    DerivedTypes& getDerivedTypes() { return this->derivedTypes; }
    const StaticString& getName() const { return this->name; }
    int32 getNameHash() const { return this->nameHash; }
    template <typename T>
    bool isKindOf() const;

    void setEditorIcon(const StaticString& icon_name) { this->editorIcon = icon_name; }
    const StaticString& getEditorIcon() const { return this->editorIcon; }
    
private:
    bool abstract;
    StaticString name;
    int32 nameHash;
    StaticString editorIcon;
    Type* base;
    DerivedTypes derivedTypes;
    Properties properties;
    PropertyOrder propertyOrder;
    PropertyOrder allPropertyOrder;
    Methods methods;
};


//-----------------------------------------------------------------------------
template <typename T>
bool Type::isKindOf() const
{
    auto cur = this;
    while (cur)
    {
        if (T::get_type() == cur) return true;
        cur = cur->base;
    }
    
    return false;
}


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
        this->setAbstract(ABSTRACT);
    }
    Object* createObject() override
    {
        return new TYPE();
    }
    void bindProperty() override
    {
        if (&TYPE::bindProperty != &BASE::bindProperty)
            TYPE::bindProperty();
    }
    void bindMethod() override
    {
        if (&TYPE::bindMethod != &BASE::bindMethod)
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
        this->setAbstract(true);
    }
    Object* createObject() override
    {
        TOD_RETURN_TRACE(nullptr);
    }
    void bindProperty() override
    {
        if (&TYPE::bindProperty != &BASE::bindProperty)
            TYPE::bindProperty();
    }
    void bindMethod() override
    {
        if (&TYPE::bindMethod != &BASE::bindMethod)
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
    Object* createObject() override
    {
        return this->createSingletonObject();
    }
    static TYPE* createSingletonObject()
    {
        static TYPE* new_obj = nullptr;
        if (nullptr == new_obj)
        {
            new_obj = new TYPE();
        }
        return new_obj;
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
    {
        TOD_THROW_EXCEPTION(
            "newInstance(%s)",
            TYPE::get_type()->getName().c_str());
    }

    return new_obj;
}
    
#define REGISTER_TYPE(type) do { type :: get_type(); } while (false)


//-----------------------------------------------------------------------------
//!@brief 특정 타입의 typeid(int64) 를 알아낸다.
//C++ 의 RTTI 가 disable 되었을때도 사용가능
//!@ingroup Core
template <typename T>
class TypeId
{
public:
    typedef TypeId<T> type;
    static int64 getId()
    {
        static std::decay<T> s_instance;
        return (int64)&s_instance;
    }
    template <typename S>
    static bool equal()
    {
        return type::getId() == TypeId<S>::getId();
    }
};


}
