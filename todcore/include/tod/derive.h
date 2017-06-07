#pragma once
#include "tod/type.h"
#include "tod/singleton.h"
namespace tod
{

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Object 상속시 사용함. @ref Reflection 시스템을 사용가능 하도록 한다
template <typename TYPE, typename BASE>
class Derive : public BASE
{
public:
    typedef BASE BaseType;
    typedef TYPE SelfType;
    
public:
    //!@brief Type을 반환
    virtual Type* getType()
    {
        return SelfType::get_type();
    }
    //!@brief Type을 반환
    virtual const Type* getType() const
    {
        return SelfType::get_type();
    }
    //!@brief Type을 반환
    static Type* get_type()
    {
        typedef ClassType<SelfType, BaseType> ObjectType;
        static ObjectType* s_type = nullptr;
        if (nullptr == s_type)
        {
            s_type = new ObjectType(
                BaseType::get_type(),
                TypeNameDemangler::getName<SelfType>());
            s_type->init();
        }
        
        return s_type;
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief 최상위 Object에 대한 Derive 특화 구현
template <typename TYPE>
class Derive<TYPE, void>
{
public:
    typedef TYPE SelfType;
    
public:
    virtual Type* getType()
    {
        return Derive<SelfType, void>::get_type();
    }
    virtual const Type* getType() const
    {
        return Derive<SelfType, void>::get_type();
    }
    static Type* get_type()
    {
        typedef ClassType<SelfType, void> ObjectType;
        static ObjectType* s_type = nullptr;
        if (nullptr == s_type)
        {
            s_type = new ObjectType(
                nullptr,
                TypeNameDemangler::getName<SelfType>());
            s_type->init();
        }
        return s_type;
    }
};
    
    
//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Singleton Object에 대한 Derive 특화 구현
template <typename TYPE, typename BASE>
class SingletonDerive : public Derive<TYPE, BASE>, ISingleton
{
public:
    typedef BASE BaseType;
    typedef TYPE SelfType;
    typedef SingletonType<SelfType, BaseType> ObjectType;
    
public:
    static Type* get_type()
    {
        static ObjectType* s_type = nullptr;
        if (nullptr == s_type)
        {
            s_type = new ObjectType(
                BaseType::get_type(),
                TypeNameDemangler::getName<SelfType>());
            s_type->init();
        }
        return s_type;
    }
};
    
}
