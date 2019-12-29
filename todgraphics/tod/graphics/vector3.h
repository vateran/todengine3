#pragma once
#include <vector>
#include <type_traits>
#include "tod/exception.h"
#include "tod/stringconv.h"
#include "tod/graphics/todmath.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
template <typename T>
class Vector3Base
{
public:
    typedef Vector3Base<T> type;
    
public:
    Vector3Base();
    Vector3Base(const T& x);
    Vector3Base(const T& x, const T& y, const T& z);
    Vector3Base(const type& other);
    
    void clear();
    float length() const;
    float lengthSquare() const;
    void normalize();
    void set(const T& x, const T& y, const T& z);
    T dot(const type& other) const;
    type cross(const type& other) const;
    float angle(const type& other) const;
    type operator - () const;
    template <typename S>
    type& operator *= (const S& value);
    template <typename S>
    type& operator /= (const S& value);
    type& operator += (const type& rhs);
    type& operator -= (const type& rhs);
    bool operator == (const type& rhs) const;
    bool operator != (const type& rhs) const;
    operator const float* () const;

    static void interpolate(float ratio, const type& v0, const type& v1, type& out);
    static void lerp(float ratio, const type& v0, const type& v1, type& out);

public:
    union
    {
        struct { T x, y, z; };
        T array[3];
    };
};
    

//-----------------------------------------------------------------------------
template <typename T>
Vector3Base<T>::Vector3Base()
{
    this->clear();
}


//-----------------------------------------------------------------------------
template <typename T>
Vector3Base<T>::Vector3Base(const T& x)
:x(x), y(x), z(x) {}


//-----------------------------------------------------------------------------
template <typename T>
Vector3Base<T>::Vector3Base(const T& x, const T& y, const T& z)
:x(x), y(y), z(z) {}
    
    
//-----------------------------------------------------------------------------
template <typename T>
Vector3Base<T>::Vector3Base(const type& other)
:x(other.x), y(other.y), z(other.z)
{
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector3Base<T>::clear()
{
    memset(this, 0, sizeof(type));
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector3Base<T>::length() const
{
    return std::sqrt(this->lengthSquare());
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector3Base<T>::lengthSquare() const
{
    return this->x * this->x + this->y * this->y + this->z * this->z;
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector3Base<T>::normalize()
{
    auto len = this->length();
    (*this) /= len;
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector3Base<T>::set(const T& x, const T& y, const T& z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


//-----------------------------------------------------------------------------
template <typename T>
T Vector3Base<T>::dot(const type& other) const
{
    return this->x * other.x + this->y * other.y + this->z * other.z;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector3Base<T>::type Vector3Base<T>::cross(const type& other) const
{
    return type(
        this->y * other.z - this->z * other.y
        , this->z * other.x - this->x * other.z
        , this->x * other.y - this->y * other.x);
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector3Base<T>::angle(const type& other) const
{
    return Math::acos(this->dot(other)
                      / (this->length() * other.length()));
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector3Base<T>::type Vector3Base<T>::operator - () const
{
    return type(-this->x, -this->y, -this->z);
}


//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
template <typename T>
typename Vector3Base<T>::type& Vector3Base<T>::operator += (const type& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector3Base<T>::type& Vector3Base<T>::operator -= (const type& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
bool Vector3Base<T>::operator == (const type& rhs) const
{
    return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
}


//-----------------------------------------------------------------------------
template <typename T>
bool Vector3Base<T>::operator != (const type& rhs) const
{
    return !(*this == rhs);
}


//-----------------------------------------------------------------------------
template <typename T>
Vector3Base<T>::operator const float* () const
{
    return this->array;
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector3Base<T>::interpolate(float ratio, const type& v0, const type& v1, type& out)
{
    Vector3Base<T>::lerp(ratio, v0, v1, out);
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector3Base<T>::lerp(float ratio, const type& v0, const type& v1, type& out)
{
    for (int32 i = 0; i < 3; ++i)
    {
        out.array[i] =
              (v0.array[i] * (1.0f - ratio))
            + (v1.array[i] * ratio);
    }
}
    

//-----------------------------------------------------------------------------
typedef Vector3Base<float> Vector3;


//-----------------------------------------------------------------------------
inline Vector3 operator + (const Vector3& v1, const Vector3& v2)
{
    return Vector3(
          v1.x + v2.x
        , v1.y + v2.y
        , v1.z + v2.z);
}


//-----------------------------------------------------------------------------
inline Vector3 operator - (const Vector3& v1, const Vector3& v2)
{
    return Vector3(
          v1.x - v2.x
        , v1.y - v2.y
        , v1.z - v2.z);
}


//-----------------------------------------------------------------------------
inline Vector3 operator * (const Vector3& v, float scalar)
{
    return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}

    
}

namespace tod
{

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Vector3 타입에 대한 String 변환
template <>
class StringConv<const graphics::Vector3&>
{
public:
    static graphics::Vector3 fromString(const String& value)
    {
        graphics::Vector3 ret;
        std::vector<String> sl;
        value.split(",", sl);
        for (auto i=0u;i<sl.size();++i)
            ret.array[i] = String::atof<float>(sl[i].c_str());
        return ret;
    }
    static void toString(const graphics::Vector3& value, String& s)
    {
        s.format("%.3f,%.3f,%.3f", value.x, value.y, value.z);
    }
};

}
