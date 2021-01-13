#pragma once
#include <stack>
namespace tod
{

class LockPolicy
{
public:
    inline void lock() { this->m.lock(); }
    inline void unlock() { this->m.unlock(); }
    std::mutex m;
};

class NoLockPolicy
{
public:
    inline void lock() {}
    inline void unlock() {}
};

template <typename LOCKPOLICY>
class LockGuard
{
public:
    explicit LockGuard(LOCKPOLICY& lock)
    : l(lock) { this->l.lock(); }
    ~LockGuard() { this->l.unlock(); }

private:
    LOCKPOLICY& l;
};


//!@ingroup Core
//!@brief 빈번히 사용되는 Object를 Pool로 만들어 관리
//! - Pool이 모자라면 자동으로 늘어남
template <typename TYPE, typename LOCKPOLICY=NoLockPolicy>
class Pool
{
private:
    typedef std::vector<void*> PtrList;
    typedef LockGuard<LOCKPOLICY> ScopedLockGuard;

public:
    ~Pool()
    {
        clear();
    }

    void clear()
    {
        for (auto p : this->all)
        {
            ::free(p);
        }

        ScopedLockGuard guard(this->lock);
        {
            PtrList e;
            this->all.swap(e);
        }
        {
            PtrList e;
            this->pool.swap(e);
        }
    }

    void reserve(size_t count)
    {
        ScopedLockGuard guard(this->lock);
        if (this->all.size() < count)
        {
            size_t expand_size = count - this->all.size();
            for (size_t i = 0; i < expand_size; ++i)
            {
                void* ret = ::malloc(size);
                this->all.emplace_back(ret);
            }
        }
    }

    template <typename ... ARGS>
    TYPE* alloc(ARGS&& ... args)
    {
        TYPE* ret = static_cast<TYPE*>(this->allocRaw(sizeof(TYPE)));
        new(ret) TYPE(std::forward<ARGS>(args) ...);
        return ret;
    }

    void dealloc(TYPE* p)
    {
        p->~TYPE();
        this->deallocRaw(p);
    }

    void* allocRaw(size_t size)
    {
        void* ret;

        ScopedLockGuard guard(this->lock);
        if (false == this->pool.empty())
        {
            ret = this->pool.back();
            this->pool.pop_back();
        }
        else
        {
            ret = ::malloc(size);
            this->all.emplace_back(ret);
        }

        return ret;
    }

    void deallocRaw(void* p)
    {
        ScopedLockGuard guard(this->lock);
        this->pool.emplace_back(p);
    }

private:
    PtrList pool;
    PtrList all;
    LOCKPOLICY lock;
};
    
}
