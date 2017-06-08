#pragma once
#include <type_traits>
#include "tod/graphics/todmath.h"
namespace tod::graphics
{


template <typename T>
class Vector3Base
{
public:
    typedef Vector3Base<T> type;
    
public:
    Vector3Base();
    Vector3Base(const T& x, const T& y, const T& z);
    Vector3Base(const type& other);
    
    void clear();
    float length() const;
    void normalize();
    float dot(const type& other) const;
    type cross(const type& other) const;
    float angle(const type& other) const;
    template <typename S>
    type& operator *= (const S& value);
    template <typename S>
    type& operator /= (const S& value);
    type& operator += (const type& rhs);
    type& operator -= (const type& rhs);
    
public:
    union
    {
        struct { T x, y, z; };
        T array[3];
    };
};
    

template <typename T>
Vector3Base<T>::Vector3Base()
{
    this->clear();
}


template <typename T>
Vector3Base<T>::Vector3Base(const T& x, const T& y, const T& z):x(x), y(y), z(z) {}
    
    
template <typename T>
Vector3Base<T>::Vector3Base(const type& other):x(other.x), y(other.y), z(other.z) {}


template <typename T>
void Vector3Base<T>::clear()
{
    memset(this, 0, sizeof(type));
}


template <typename T>
float Vector3Base<T>::length() const
{
    return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}


template <typename T>
void Vector3Base<T>::normalize()
{
    auto len = this->length();
    (*this) /= len;
}


template <typename T>
float Vector3Base<T>::dot(const type& other) const
{
    return type(this->x * other.x, this->y * other.y, this->z * other.z);
}


template <typename T>
typename Vector3Base<T>::type Vector3Base<T>::cross(const type& other) const
{
    return Vector3(
                   this->y * other.z - this->z * other.y
                   , this->z * other.x - this->x * other.z
                   , this->x * other.y - this->y * other.x);
}

    
template <typename T>
float Vector3Base<T>::angle(const type& other) const
{
    return Math::acos(this->dot(other)
                      / (this->length() * other.length()));
}
    
    
template <typename T>
template <typename S>
typename Vector3Base<T>::type& Vector3Base<T>::operator *= (const S& value)
{
    static_assert(std::is_scalar<S>::value, "S must be of scalar type");
    this->x *= value;
    this->y *= value;
    this->z *= value;
    return *this;
}
    
    
template <typename T>
template <typename S>
typename Vector3Base<T>::type& Vector3Base<T>::operator /= (const S& value)
{
    static_assert(std::is_scalar<S>::value, "S must be of scalar type");
    this->x /= value;
    this->y /= value;
    this->z /= value;
    return *this;
}
    
    
template <typename T>
typename Vector3Base<T>::type& Vector3Base<T>::operator += (const type& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}
    
    
template <typename T>
typename Vector3Base<T>::type& Vector3Base<T>::operator -= (const type& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}
    

typedef Vector3Base<float> Vector3;
    
}
