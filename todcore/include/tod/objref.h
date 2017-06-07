#pragma once
#include "tod/object.h"
namespace tod
{
    
class ObjRefHelper
{
public:
    static void setByPath(Object** ptr, const String& path);
};

//!@ingroup Core
//!@brief Object 사용시, reference count 를 자동으로 관리하는 guard 클래스
//!@code
//!ObjRef<Node> foo(Kernel::instance()->lookup<Node>("/node1");
//!foo->call();
//!@endcode
template <typename T>
class ObjRef
{
public:
    typedef ObjRef<T> type;
    
public:
    ObjRef(T* object):target(nullptr)
    {
        this->set(object);
    }
    ObjRef(const ObjRef<T>& ref):target(nullptr)
    {
        this->set(ref.target);
    }
    ObjRef(const String& path):target(nullptr)
    {
        this->setByPath(path);
    }
    ~ObjRef()
    {
        if (this->target)
            this->target->release();
    }
    
    T* operator -> ()
    {
        return this->target;
    }
    
    const T* operator -> () const
    {
        return this->target;
    }
    
    T* operator * ()
    {
        return this->target;
    }
    
    bool valid() const
    {
        return this->target != nullptr;
    }
    
    void set(T* object)
    {
        if (nullptr != this->target)
            this->target->release();
        this->target = object;
        if (nullptr != this->target)
            this->target->retain();
    }
    
    void setByPath(const String& path)
    {
        Object* obj = nullptr;
        ObjRefHelper::setByPath(&obj, path);
        this->set(static_cast<T*>(obj));
    }
    
    T* get()
    {
        return this->target;
    }
    
    const T* get() const
    {
        return this->target;
    }
    
    T* operator = (T* rhs)
    {
        this->set(rhs);
        return this->target;
    }
    
    operator T* ()
    {
        return this->target;
    }
    
    bool operator == (const T* rhs) const
    {
        return this->target == rhs;
    }
    
    type& operator = (const String& path)
    {
        this->setByPath(path);
        return *this;
    }
    
private:
    T* target;
};
    
}
