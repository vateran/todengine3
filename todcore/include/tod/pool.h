#pragma once
#include <stack>
namespace tod
{
 
//!@ingroup Core
//!@brief 빈번히 사용되는 Object를 Pool로 만들어 관리
//! - Pool이 모자라면 자동으로 늘어남
template <typename TYPE, size_t GROW=1>
class Pool
{
public:
    virtual~Pool()
    {
        this->clear();
    }
    void clear()
    {
        while (!this->pool.empty())
        {
            free(this->pool.top());
            this->pool.pop();
        }
    }
    TYPE* alloc()
    {
        if (this->pool.empty()) this->grow(GROW);
        auto new_obj = this->pool.top();
        this->pool.pop();
        return new_obj;
    }
    void dealloc(TYPE* object)
    {
        this->pool.push(object);
    }
    
private:
    void grow(size_t n)
    {
        for (size_t i=0;i<n;++i)
        {
            auto p = static_cast<TYPE*>(malloc(sizeof(TYPE)));
            this->pool.push(p);
        }
    }
    
private:
    std::stack<TYPE*> pool;
};
    
}
