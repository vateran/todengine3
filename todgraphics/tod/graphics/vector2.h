#pragma once
#include <type_traits>
#include "tod/stringconv.h"
#include "tod/graphics/todmath.h"
namespace tod::graphics
{


//-----------------------------------------------------------------------------
template <typename T>
class Vector2Base
{
public:
    typedef Vector2Base<T> type;
    
public:
    Vector2Base();
    Vector2Base(const T& x);
    Vector2Base(const T& x, const T& y);
    Vector2Base(const type& other);
    
    void clear();
    float length() const;
    void normalize();
    void set(const T& x, const T& y);
    float dot(const type& other) const;
    float cross(const type& other) const;
    float angle(const type& other) const;
    template <typename S>
    type& operator *= (const S& value);
    template <typename S>
    type& operator /= (const S& value);
    type& operator += (const type& rhs);
    type& operator -= (const type& rhs);
    bool operator == (const type& rhs) const;
    bool operator != (const type& rhs) const;
    
public:
    union
    {
        struct { T x, y; };
        T array[2];
    };
};
    

//-----------------------------------------------------------------------------
template <typename T>
Vector2Base<T>::Vector2Base()
{
    this->clear();
}


//-----------------------------------------------------------------------------
template <typename T>
Vector2Base<T>::Vector2Base(const T& x)
: x(x), y(x) {}


//-----------------------------------------------------------------------------
template <typename T>
Vector2Base<T>::Vector2Base(const T& x, const T& y)
: x(x), y(y) {}
    
    
//-----------------------------------------------------------------------------
template <typename T>
Vector2Base<T>::Vector2Base(const type& other)
: x(other.x), y(other.y) {}


//-----------------------------------------------------------------------------
template <typename T>
void Vector2Base<T>::clear()
{
    memset(this, 0, sizeof(type));
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector2Base<T>::length() const
{
    return std::sqrt(this->x * this->x + this->y * this->y);
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector2Base<T>::normalize()
{
    auto len = this->length();
    (*this) /= len;
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector2Base<T>::set(const T& x, const T& y)
{
    this->x = x;
    this->y = y;
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector2Base<T>::dot(const type& other) const
{
    return type(this->x * other.x, this->y * other.y);
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector2Base<T>::cross(const type& other) const
{
    return (this->x * other.y) - (this->Y * other.x);
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector2Base<T>::angle(const type& other) const
{
    return Math::acos(this->dot(other)
                      / (this->length() * other.length()));
}


//-----------------------------------------------------------------------------
template <typename T>
template <typename S>
typename Vector2Base<T>::type& Vector2Base<T>::operator *= (const S& value)
{
    static_assert(std::is_scalar<S>::value, "S must be of scalar type");
    this->x *= value;
    this->y *= value;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
template <typename S>
typename Vector2Base<T>::type& Vector2Base<T>::operator /= (const S& value)
{
    static_assert(std::is_scalar<S>::value, "S must be of scalar type");
    this->x /= value;
    this->y /= value;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector2Base<T>::type& Vector2Base<T>::operator += (const type& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector2Base<T>::type& Vector2Base<T>::operator -= (const type& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
bool Vector2Base<T>::operator == (const type& rhs) const
{
    return this->x == rhs.x && this->y == rhs.y;
}


//-----------------------------------------------------------------------------
template <typename T>
bool Vector2Base<T>::operator != (const type& rhs) const
{
    return !(*this == rhs);
}
    

//-----------------------------------------------------------------------------
typedef Vector2Base<float> Vector2;
    
}

namespace tod
{

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Vector2 타입에 대한 String 변환
template <>
class StringConv<const graphics::Vector2&>
{
public:
    static graphics::Vector2 fromString(const String& value)
    {
        graphics::Vector2 ret;
        std::vector<String> sl;
        value.split(",", sl);
        for (auto i=0u;i<sl.size();++i)
            ret.array[i] = String::atof<float>(sl[i].c_str());
        return ret;
    }
    static void toString(const graphics::Vector2& value, String& s)
    {
        s.format("%.3f,%.3f", value.x, value.y);
    }
};

}
