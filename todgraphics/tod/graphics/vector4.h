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
class Vector4Base
{
public:
    typedef Vector4Base<T> type;
    
public:
    Vector4Base();
    Vector4Base(const T& x, const T& y, const T& z, const T& w);
    Vector4Base(const type& other);
    
    void clear();
    float length() const;
    void normalize();
    float dot(const type& other) const;
    float angle(const type& other) const;
    typename Vector3Base<T>::type toEuler();
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
        struct { T x, y, z, w; };
        T array[4];
    };
};


//-----------------------------------------------------------------------------
template <typename T>
Vector4Base<T>::Vector4Base()
{
    this->clear();
}


//-----------------------------------------------------------------------------
template <typename T>
Vector4Base<T>::Vector4Base(const T& x, const T& y, const T& z, const T& w):
x(x), y(y), z(z), w(w) {}


//-----------------------------------------------------------------------------
template <typename T>
Vector4Base<T>::Vector4Base(const type& other)
:x(other.x), y(other.y), z(other.z), w(other.w) {}


//-----------------------------------------------------------------------------
template <typename T>
void Vector4Base<T>::clear()
{
    memset(this, 0, sizeof(type));
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector4Base<T>::length() const
{
    return std::sqrt(this->x * this->x
                     + this->y * this->y
                     + this->z * this->z
                     + this->w * this->w);
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector4Base<T>::normalize()
{
    auto len = this->length();
    (*this) /= len;
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector4Base<T>::dot(const type& other) const
{
    return type(this->x * other.x, this->y * other.y,
                this->z * other.z, this->w * other.w);
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector4Base<T>::angle(const type& other) const
{
    return Math::acos(this->dot(other)
                      / (this->length() * other.length()));
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector3Base<T>::type Vector4Base<T>::toEuler()
{
    T y_square = this->y * this->y;
    
    // roll (x-axis rotation)
	T t0 = +2.0 * (this->w * this->x + this->y * this->z);
	T t1 = +1.0 - 2.0 * (this->x * this->x + y_square);
	T roll = Math::atan2(t0, t1);

	// pitch (y-axis rotation)
	T t2 = +2.0 * (this->w * this->y - this->z * this->x);
	t2 = ((t2 > 1.0) ? 1.0 : t2);
	t2 = ((t2 < -1.0) ? -1.0 : t2);
	T pitch = Math::asin(t2);

	// yaw (z-axis rotation)
	T t3 = +2.0 * (this->w * this->z + this->x * this->y);
	T t4 = +1.0 - 2.0 * (y_square + this->z * this->z);
	T yaw = Math::atan2(t3, t4);
    
    return typename Vector3Base<T>::type(pitch, yaw, roll);
}


//-----------------------------------------------------------------------------
template <typename T>
template <typename S>
typename Vector4Base<T>::type& Vector4Base<T>::operator *= (const S& value)
{
    static_assert(std::is_scalar<S>::value, "S must be of scalar type");
    this->x *= value;
    this->y *= value;
    this->z *= value;
    this->w *= value;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
template <typename S>
typename Vector4Base<T>::type& Vector4Base<T>::operator /= (const S& value)
{
    static_assert(std::is_scalar<S>::value, "S must be of scalar type");
    this->x /= value;
    this->y /= value;
    this->z /= value;
    this->w /= value;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector4Base<T>::type& Vector4Base<T>::operator += (const type& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    this->w += rhs.w;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector4Base<T>::type& Vector4Base<T>::operator -= (const type& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    this->w -= rhs.w;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
bool Vector4Base<T>::operator == (const type& rhs) const
{
    return this->x == rhs.x && this->y == rhs.y
        && this->z == rhs.z && this->w == rhs.w;
}


//-----------------------------------------------------------------------------
template <typename T>
bool Vector4Base<T>::operator != (const type& rhs) const
{
    return !(*this == rhs);
}


//-----------------------------------------------------------------------------
typedef Vector4Base<float> Vector4;
typedef Vector4Base<float> Quaternion;


}


namespace tod
{

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Vector4 타입에 대한 String 변환
template <>
class StringConv<const graphics::Vector4&>
{
public:
    static graphics::Vector4 fromString(const String& value)
    {
        graphics::Vector4 ret;
        std::vector<String> sl;
        value.split(",", sl);
        for (auto i=0u;i<sl.size();++i)
        {
            ret.array[i] = String::atof<float>(sl[i].c_str());
        }
        return ret;
    }
    static void toString(const graphics::Vector4& value, String& s)
    {
        s.format("%.3f,%.3f,%.3f,%.3f", value.x, value.y, value.z, value.w);
    }
};

}
