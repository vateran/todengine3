#pragma once
#include <vector>
#include "tod/any.h"
#include "tod/pool.h"
#include "tod/singleton.h"
namespace tod
{
    
//!@ingroup Core
//!@brief 시스템 내에서 동적으로 조합된 데이터들을 Hold 할 수 있음
class Params
{
public:
    Params() { this->clear(); }
    Params(Params&& param)
    {
        std::swap(this->data, param.data);
    }
    inline bool empty() const { return this->data.empty(); }
    inline size_t size() const { return this->data.size(); }
    inline void reserve(size_t n) { this->data.reserve(n); }
    inline void clear() { this->data.clear(); }
    inline void push_back(const std::any& value) { this->data.push_back(std::move(value)); }
    inline void push_back(std::any&& value) { this->data.push_back(std::move(value)); }
    inline std::any& at(size_t index) { return this->data.at(index); }
    inline const std::any& at(size_t index) const { return this->data.at(index); }
    inline std::any& operator [] (size_t index) { return this->data.at(index); }
    inline const std::any& operator [] (size_t index) const { return this->data.at(index); }
    
    //!@brief 지정된 인자로 새로운 Params 를 구성하고 반환
    //! Params는 내부적으로 pool을 사용하여 객체들을 관리한다
    template <typename ... ARGS>
    static Params* newParam(const ARGS& ... args)
    {
        Params* params = new Params();
        params->reserve(sizeof ... (args));
        build_param(*params, args ...);
        return params;
    }
    
public:
    void* operator new (size_t bytes)
    {
        return ParamPool::instance()->alloc();
    }
    
    void operator delete (void* param)
    {
        ParamPool::instance()->dealloc((Params*)param);
    }
    
private:
    std::vector<std::any> data;
    
private:
    Params(const Params&)=delete;
    Params& operator = (const Params&)=delete;
    void* operator new [] (size_t bytes)=delete;
    void operator delete [] (void* param)=delete;
    
    static void build_param(Params& out) {}
    template <typename ARG, typename ... ARGS>
    static void build_param(Params& out, const ARG& arg, const ARGS& ... args)
    {
        out.push_back(arg);
        build_param(out, args ...);
    }
    class ParamPool : public Singleton<ParamPool>, public Pool<Params> {};
};
    
}
