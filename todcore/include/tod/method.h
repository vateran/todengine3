#pragma once
#include <vector>
#include "tod/params.h"
namespace tod
{


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Object 의 method 를 추상화
class Method
{
public:
    Method(const String& name):name(name) {}
    virtual~Method() {}
    
    
    //!@brief Method 를 실행한다. 실행전, input parameters들을 설정하는 것을 권장한다
    virtual void invoke(Object* self)=0;
    
    
    //!@brief Method 실행시 입력되는 input parameters
    Params& in() { return this->inParam; }
    //!@brief Method가 실행후 출력되는 output parameters
    Params& out() { return this->outParam; }
    
    
    inline const String& getName() const { return this->name; }
    inline void setDesc(const String& value) { this->desc = value; }
    inline const String& getDesc() const { return this->desc; }
    
private:
    String name;
    String desc;
    Params inParam;
    Params outParam;
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Object 의 method 를 Type에 등록하기 위한 Holder 템플릿
template <typename TYPE>
class MethodBind : public Method
{
public:
    typedef std::function<void(TYPE* self, Params& in, Params& out)> Func;
    
public:
    MethodBind(const String& name):Method(name) {}
    
    
    //!@brief Method 와 실제 객체method 를 연결한다
    template <typename ... ARGS>
    void bind(const Func& func, const ARGS ... args)
    {
        this->func = func;
        
        InitParam init_param(this);
        init_param.add(args ...);
    }
    
    
    //!@brief Method 를 실행한다
    void invoke(Object* self) override
    { this->func(static_cast<TYPE*>(self), this->in(), this->out()); }
    
private:
    struct InitParam
    {
        InitParam(Method* method):method(method) {}
        
        void add() {}
        template <typename ARG_TYPE, typename ... ARGS>
        void add(const ARG_TYPE& arg, const ARGS& ... args)
        {
            method->in().push_back(arg);
            this->add(args ...);
        }
        Method* method;
    };
    
private:
    Func func;
    
};


//-----------------------------------------------------------------------------
#define BIND_METHOD(name, desc, func_body, ...) do {\
typedef MethodBind<SelfType> MethodType;\
auto method = new MethodType(name);\
method->setDesc(desc);\
method->bind([](SelfType* self, Params& in, Params& out) func_body, ## __VA_ARGS__);\
SelfType::get_type()->addMethod(method); } while (false)
    
}
