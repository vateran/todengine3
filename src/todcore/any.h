#pragma once
#include "tod/exception.h"
namespace std
{
    //XCODE 의 C++17 이 활성화 될때까지 사용
    class any
    {
    public:
        any():content(nullptr) {}
        template <typename T>
        any(const T& value):content(new Holder<T>(value)) {}
        any(const any& other):
        content(other.content?other.content->clone():nullptr) { }
        any(any&& other):content(other.content) { other.content = nullptr; }
        ~any() { if (nullptr != this->content) delete this->content; }
        
        any& swap(any& rhs)
        {
            std::swap(this->content, rhs.content);
            return *this;
        }
        
        any& operator = (const any& rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }
        
        any& operator = (any&& rhs)
        {
            rhs.swap(*this);
            any().swap(rhs);
            return *this;
        }
        
        bool empty() const
        {
            return this->content == nullptr;
        }
        
        void clear()
        {
            any().swap(*this);
        }
        
        const std::type_info& type() const
        {
            return this->content ? this->content->type() : typeid(void);
        }
        
    private:
        class PlaceHoler
        {
        public:
            virtual~PlaceHoler() {}
            virtual const std::type_info& type() const = 0;
            virtual PlaceHoler* clone() const = 0;
        };
        
        template <typename T>
        class Holder : public PlaceHoler
        {
        public:
            Holder(const T& value):data(value) {}
            const std::type_info& type() const override
            { return typeid(T); }
            PlaceHoler* clone() const override
            { return new Holder<T>(this->data); }
            
        private:
            Holder& operator = (const Holder&);
            
        public:
            T data;
        };
        
    private:
        PlaceHoler* content;
        
        template <typename T>
        friend T any_cast(const any& operand);
    };
    
    inline void swap(any& lhs, any& rhs) { lhs.swap(rhs); }
    
    template <typename T>
    T any_cast(const any& operand)
    {
        typedef any::Holder<T> HolderType;
        if (typeid(T) != operand.type()) TOD_THROW_EXCEPTION("bad_any_cast");
        auto holder = static_cast<HolderType*>(operand.content);
        return holder->data;
    }
}
