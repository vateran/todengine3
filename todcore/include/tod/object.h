#pragma once
#include "tod/string.h"
#include "tod/uuid.h"
#include "tod/derive.h"
#include "tod/property.h"
#include "tod/method.h"
namespace tod
{

//!@ingroup Core
//!@brief TodEngine에서 사용하는 모든 객체의 최상위 클래스
//!- Refence Count 기반의 수명관리
//!- 객체의 Type @ref Reflection 제공
class Object : public Derive<Object, void>
{
public:
    Object();
    virtual~Object() {}
    
    void retain();
    int release();
    inline int getRefCount() const { return this->refCount; }
    
    Property* findProperty(const String& prop_name);
    void resetAllProperties();
    Method* findMethod(const String& method_name);
    
    static void bindProperty();
    static void bindMethod() {}
    
private:
    int refCount;
};

}
