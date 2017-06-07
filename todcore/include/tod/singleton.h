#pragma once
#include <string>
#include "tod/platformdef.h"
#include "tod/type.h"
#include "tod/singletonmgr.h"
namespace tod
{

//-----------------------------------------------------------------------------
//!@ingroup Core
//!@brief TodEngine 에서 Singleton을 구현할때 사용하는 인터페이스
//!시스템 종료시 Singleton 객체들은 추가된 Singleton의 역순으로 파괴됨
class ISingleton
{
public:
    virtual~ISingleton() {}
    //!@brief Singleton 객체의 이름을 반환
    virtual const std::string& getName()=0;
};


//-----------------------------------------------------------------------------
//!@ingroup Core
//!@brief TodEngine 에서 Singleton을 구현할때 상속하는 템플릿
//!@code
//!class Foo : public Singleton<Foo> {};
//!Foo::instance()->call();
//!@endcode
template <typename T>
class Singleton : public ISingleton
{
public:
    Singleton()
    {
        this->name = TypeNameDemangler::getName<T>();
        SingletonMgr::instance()->push_back(this);
    }
    const std::string& getName() override { return this->name; }
    //!@brief Singleton 객체를 반환
    static T* instance()
    {
        static T* s_object = nullptr;
        if (nullptr == s_object)
            s_object = new T();
        return s_object;
    }
    
private:
    std::string name;
};

}
