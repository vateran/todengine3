#pragma once
#include <mutex>
#include "tod/basetype.h"
namespace tod
{

namespace policy
{
    struct LockPolicy
    {
    public:
        std::mutex* getMutex() { return &this->m; }

    private:
        std::mutex m;
    };

    struct NoLockPolicy
    {
    public:
        std::mutex* getMutex() { return nullptr; }
    };

    struct LockGuard
    {
    public:
        LockGuard(std::mutex* m)
            : m(m)
        {
            if (nullptr != this->m)
            {
                this->m->lock();
            }
        }
        ~LockGuard()
        {
            if (nullptr != this->m)
            {
                this->m->unlock();
            }
        }
        std::mutex* m;
    };
}

template <typename TYPE, typename INDEX_TYPE=uint32, typename LOCK_POLICY=policy::NoLockPolicy>
class IndexedPool
{    
public:
    IndexedPool()
    {
        this->clear();
    }
    ~IndexedPool()
    {
        this->clear();
    }

    void clear()
    {
        policy::LockGuard guard(this->lockPolicy.getMutex());

        for (size_t i = 0; i < this->pool.size(); ++i)
        {
            TOD_SAFE_DELETE(this->pool[i]);
        }

        this->pool.clear();
        this->deallocList.clear();
        //index[0] 은 사용하지 않는다
        this->pool.emplace_back(nullptr);
    }

    INDEX_TYPE size() const
    {
        policy::LockGuard guard(this->lockPolicy.getMutex());

        return static_cast<INDEX_TYPE>
            (this->pool.size() - this->deallocList.size());
    }

    //@brief pool 에서 인덱스를 할당
    INDEX_TYPE alloc()
    {
        policy::LockGuard guard(this->lockPolicy.getMutex());

        if (true == this->deallocList.empty())
        {
            this->pool.emplace_back(new TYPE);
            return static_cast<INDEX_TYPE>(this->pool.size() - 1);
        }
        else
        {
            const INDEX_TYPE index = this->deallocList.back();
            return index;
        }
    }

    void dealloc(INDEX_TYPE index)
    {
        policy::LockGuard guard(this->lockPolicy.getMutex());

        this->deallocList.emplace_back(index);
    }

    void deallocAll()
    {
        policy::LockGuard guard(this->lockPolicy.getMutex());

        this->deallocList.clear();

        INDEX_TYPE size = static_cast<INDEX_TYPE>(this->pool.size());
        for (INDEX_TYPE i = 0; i < size; ++i)
        {
            this->deallocList.emplace_back(i);
        }
    }

    TYPE* get(INDEX_TYPE index)
    {
        return this->pool[index];
    }

    const TYPE* get(INDEX_TYPE index) const
    {
        return this->pool[index];
    }

private:
    std::vector<TYPE*> pool;
    std::vector<INDEX_TYPE> deallocList;
    LOCK_POLICY lockPolicy;
};

}
