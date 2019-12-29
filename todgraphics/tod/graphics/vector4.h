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
    typedef typename Vector3Base<T>::type vec3;
    
public:
    Vector4Base();
    Vector4Base(const T& x);
    Vector4Base(const T& x, const T& y, const T& z, const T& w);
    Vector4Base(const type& other);
    
    void clear();
    T lengthSquare() const;
    T length() const;
    void normalize();
    float dot(const type& other) const;
    float dot(const vec3& other) const;
    float angle(const type& other) const;
    void set(const T& x, const T& y, const T& z, const T& w);
    
    vec3 toEuler();
    void transform(vec3& inout);

    template <typename S>
    type& operator *= (const S& value);
    template <typename S>
    type& operator /= (const S& value);
    type& operator += (const type& rhs);
    type& operator -= (const type& rhs);
    bool operator == (const type& rhs) const;
    bool operator != (const type& rhs) const;
    type operator + (type& rhs);
    type operator - (type& rhs);
    template <typename S>
    type& operator * (const S& value);
    type& operator - ();

    static void interpolate(float ratio, const type& v0, const type& v1, type& out);
    static void slerp(float ratio, const type& v0, const type& v1, type& out);
    
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
Vector4Base<T>::Vector4Base(const T& x)
: x(x), y(x), z(x), w(x) {}


//-----------------------------------------------------------------------------
template <typename T>
Vector4Base<T>::Vector4Base(const T& x, const T& y, const T& z, const T& w)
: x(x), y(y), z(z), w(w) {}


//-----------------------------------------------------------------------------
template <typename T>
Vector4Base<T>::Vector4Base(const type& other)
: x(other.x), y(other.y), z(other.z), w(other.w) {}


//-----------------------------------------------------------------------------
template <typename T>
void Vector4Base<T>::clear()
{
    memset(this, 0, sizeof(type));
}


//-----------------------------------------------------------------------------
template <typename T>
T Vector4Base<T>::length() const
{
    return std::sqrt(this->lengthSquare());
}


//-----------------------------------------------------------------------------
template <typename T>
T Vector4Base<T>::lengthSquare() const
{
    return this->x * this->x
        + this->y * this->y
        + this->z * this->z
        + this->w * this->w;
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
    return (this->x * other.x)
         + (this->y * other.y)
         + (this->z * other.z)
         + (this->w * other.w);
}


//-----------------------------------------------------------------------------
template <typename T>
float Vector4Base<T>::dot(const vec3& other) const
{
    return (this->x * other.x)
         + (this->y * other.y)
         + (this->z * other.z)
         + this->w;
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
void Vector4Base<T>::set(const T& x, const T& y, const T& z, const T& w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector3Base<T>::type Vector4Base<T>::toEuler()
{
    T y_square = this->y * this->y;
    
    // roll (x-axis rotation)
    T t0 = static_cast<T>(+2.0 * (this->w * this->x + this->y * this->z));
    T t1 = static_cast<T>(+1.0 - 2.0 * (this->x * this->x + y_square));
    T roll = Math::atan2(t0, t1);

    // pitch (y-axis rotation)
    T t2 = static_cast<T>(+2.0 * (this->w * this->y - this->z * this->x));
    t2 = static_cast<T>((t2 > 1.0) ? 1.0 : t2);
    t2 = static_cast<T>((t2 < -1.0) ? -1.0 : t2);
    T pitch = Math::asin(t2);

    // yaw (z-axis rotation)
    T t3 = static_cast<T>(+2.0 * (this->w * this->z + this->x * this->y));
    T t4 = static_cast<T>(+1.0 - 2.0 * (y_square + this->z * this->z));
    T yaw = Math::atan2(t3, t4);
    
    return typename Vector3Base<T>::type(pitch, yaw, roll);
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector4Base<T>::transform(vec3& inout)
{
    T x2 = this->x + this->x;
    T y2 = this->y + this->y;
    T z2 = this->z + this->z;
    T wx2 = this->w * x2;
    T wy2 = this->w * y2;
    T wz2 = this->w * z2;
    T xx2 = this->x * x2;
    T xy2 = this->x * y2;
    T xz2 = this->x * z2;
    T yy2 = this->y * y2;
    T yz2 = this->y * z2;
    T zz2 = this->z * z2;
    T x = inout.x * (1.0 - yy2 - zz2)
        + inout.y * (xy2 - wz2)
        + inout.z * (xz2 + wy2);
    T y = inout.x * (xy2 + wz2)
        + inout.y * (1.0 - xx2 - zz2)
        + inout.z * (yz2 - wx2);
    T z = inout.x * (xz2 - wy2)
        + inout.y * (yz2 + wx2)
        + inout.z * (1.0 - xx2 - yy2);
    inout.x = x;
    inout.y = y;
    inout.z = z;
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
template <typename T>
typename Vector4Base<T> Vector4Base<T>::operator + (Vector4Base<T>& other)
{
    return Vector4Base<T>(
          this->x + other.x
        , this->y + other.y
        , this->z + other.z
        , this->w + other.w);
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector4Base<T> Vector4Base<T>::operator - (Vector4Base<T>& other)
{
    return Vector4Base<T>(
          this->x - other.x
        , this->y - other.y
        , this->z - other.z
        , this->w - other.w);
}


//-----------------------------------------------------------------------------
template <typename T>
typename Vector4Base<T>& Vector4Base<T>::operator - ()
{
    *this *= -1;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
template <typename S>
typename Vector4Base<T>& Vector4Base<T>::operator * (const S& value)
{
    *this *= value;
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector4Base<T>::interpolate
(float ratio, const type& v0, const type& v1, type& out)
{
    Vector4Base<T>::slerp(ratio, v0, v1, out);
}


//-----------------------------------------------------------------------------
template <typename T>
void Vector4Base<T>::slerp
(float ratio, const type& v0, const type& v1, type& out)
{
    float cosom = v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
    Vector4Base<T> end = v1;
    if (cosom < 0.0f)
    {
        cosom = -cosom;
        end.x = -end.x;
        end.y = -end.y;
        end.z = -end.z;
        end.w = -end.w;
    }

    float sclp, sclq;
    if ((1.0f - cosom) > 0.0001f)
    {
        float omega, sinom;
        omega = Math::acos(cosom);
        sinom = Math::sin(omega);
        sclp = Math::sin((1.0f - ratio) * omega) / sinom;
        sclq = Math::sin(ratio * omega) / sinom;
    }
    else
    {
        sclp = 1.0f - ratio;
        sclq = ratio;
    }

    out.x = sclp * v0.x + sclq * end.x;
    out.y = sclp * v0.y + sclq * end.y;
    out.z = sclp * v0.z + sclq * end.z;
    out.w = sclp * v0.w + sclq * end.w;
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
