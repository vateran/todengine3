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
    ObjRef():target(nullptr)
    {
    }
    ObjRef(T* object):target(nullptr)
    {
        this->set(object);
    }
    ObjRef(const type& ref):target(nullptr)
    {
        this->set(ref.target);
    }
    ObjRef(type&& ref):target(nullptr)
    {
        std::swap(this->target, ref.target);
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
    
    void release()
    {
        if (this->target)
            this->target->release();
        this->target = nullptr;
    }
    
    inline T* operator -> ()
    {
        return this->target;
    }
    
    inline const T* operator -> () const
    {
        return this->target;
    }
    
    inline T* operator * ()
    {
        return this->target;
    }
    
    inline bool valid() const
    {
        return nullptr != this->target;
    }
    
    inline bool invalid() const
    {
        return nullptr == this->target;
    }
    
    void set(T* object)
    {
        this->release();
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
    
    inline T* get()
    {
        return this->target;
    }
    
    inline const T* get() const
    {
        return this->target;
    }
    
    inline bool equal(const Object* other) const
    {
        return this->target == other;
    }
    inline bool equal(const type& other) const
    {
        return this->target == other.target;
    }
    
    inline type& operator = (const type& rhs)
    {
        this->set(rhs.target);
        return *this;
    }
    
    inline type& operator = (type&& rhs)
    {
        std::swap(this->target, rhs.target);
        return *this;
    }
    
    inline type& operator = (const String& path)
    {
        this->setByPath(path);
        return *this;
    }
    
    inline T* operator = (T* rhs)
    {
        this->set(rhs);
        return this->target;
    }
    
    template <typename S>
    inline operator S* ()
    {
        return static_cast<S*>(this->target);
    }
    
    type& operator = (const T* rhs)
    {
        this->set(rhs);
        return *this;
    }
    
    template <typename S>
    inline bool operator == (const S* rhs) const
    {
        return this->target == rhs;
    }
    
    template <typename S>
    inline bool operator != (const S* rhs) const
    {
        return this->target != rhs;
    }
    inline bool operator == (const type& rhs) const
    {
        return this->target == rhs.target;
    }
    inline bool operator != (const type& rhs) const
    {
        return this->target != rhs.target;
    }
    
private:
    T* target;
};
    
}
