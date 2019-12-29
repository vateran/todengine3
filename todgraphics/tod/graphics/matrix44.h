#pragma once
#include "tod/graphics/vector3.h"
#include "tod/graphics/vector4.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
template <typename T>
class Matrix44Base
{
public:
    typedef Matrix44Base<T> type;
    
public:
    Matrix44Base();
    Matrix44Base(bool identity);
    Matrix44Base(const Quaternion& r);
    Matrix44Base(const type& value);
    
    void clear();
    void identity();
    void setTranslation(const Vector3& t);
    Vector3 getTranslation() const;
    void setEulerRotation(const Vector3& r);
    void setScaling(const Vector3& s);
    void translate(const Vector3& t);
    void scale(const Vector3& s);
    void eulerRotate(const Vector3& r);
    float determinent();
    bool inverse();
    bool inverseTo(type& out);
    void transpose();
    void makeFromSRT(const Vector3& s, const Quaternion& r, const Vector3& t);
    void makeFromSRTTranspose(const Vector3& s, const Quaternion& r, const Vector3& t);
    void makeFromQuaternion(const Quaternion& r);
    Vector3 getRightVector();
    Vector3 getUpVector();
    Vector3 getForwardVector();
    
    void lookAt(const Vector3& target);
    void lookAt(const Vector3& target, const Vector3& up);
    Vector3 getAngle();
    
    void perspectiveLH(T fov, T aspect, T zNear, T zFar);
    void perspectiveRH(T fov, T aspect, T zNear, T zFar);
    
    void orthogonalLH(T left, T right, T bottom, T top, T zNear, T zFar);
    void orthogonalRH(T left, T right, T bottom, T top, T zNear, T zFar);
    
    void setX(T value);
    void setY(T value);
    void setZ(T value);
    T getX();
    T getY();
    T getZ();

    T& m(int32 row, int32 column);
    const T& m(int32 row, int32 column) const;
    
    type& operator += (const type& other);
    type& operator -= (const type& other);
    type& operator *= (const type& other);
    type operator * (const type& other) const;

    operator float* ();
    
public:
    union
    {
        struct
        {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        T d[4][4];
        T array[16];
        Vector4 v[4];
    };
};


//-----------------------------------------------------------------------------
template <typename T>
Matrix44Base<T>::Matrix44Base()
{
}


//-----------------------------------------------------------------------------
template <typename T>
Matrix44Base<T>::Matrix44Base(bool identity)
{
    if (identity)
    {
        this->identity();
    }
}


//-----------------------------------------------------------------------------
template <typename T>
Matrix44Base<T>::Matrix44Base(const Quaternion& r)
{
    this->makeFromQuaternion(r);
}


//-----------------------------------------------------------------------------
template <typename T>
Matrix44Base<T>::Matrix44Base(const type& value)
{
    memcpy(this->array, value.array, sizeof(type));
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::clear()
{
    memset(this->d, 0, sizeof(type));
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::identity()
{
    this->clear();
    for (int32 i = 0; i < 4; ++i)
    {
        d[i][i] = 1;
    }
}
    

//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::setTranslation(const Vector3& t)
{
    this->identity();
    this->m41 = t.x;
    this->m42 = t.y;
    this->m43 = t.z;
}


//-----------------------------------------------------------------------------
template <typename T>
Vector3 Matrix44Base<T>::getTranslation() const
{
    return Vector3(this->m41, this->m42, this->m43);
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::setEulerRotation(const Vector3& r)
{
    this->clear();
    auto sin_rx = Math::sin(r.x);
    auto cos_rx = Math::cos(r.x);
    
    auto sin_ry = Math::sin(r.y);
    auto cos_ry = Math::cos(r.y);
    
    auto sin_rz = Math::sin(r.z);
    auto cos_rz = Math::cos(r.z);
    
    this->m11 = cos_ry * cos_rz;
    this->m12 = -cos_ry * sin_rz;
    this->m13 = sin_ry;

    this->m21 = cos_rx * sin_rz + sin_rx * sin_ry * cos_rz;
    this->m22 = cos_rx * cos_rz - sin_rx * sin_ry * sin_rz;
    this->m23 = -sin_rx * cos_ry;

    this->m31 = sin_rx * sin_rz - cos_rx * sin_ry * cos_rz;
    this->m32 = sin_rx * cos_rz + cos_rx * sin_ry * sin_rz;
    this->m33 = cos_rx * cos_ry;

    this->m44 = 1.0f;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::setScaling(const Vector3& s)
{
    this->clear();
    this->m11 = s.x;
    this->m22 = s.y;
    this->m33 = s.z;
    this->m44 = 1.0f;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::translate(const Vector3& t)
{
    auto x = t.x, y = t.y, z = t.z;
    this->array[0] += this->array[3] * x;   this->array[4] += this->array[7] * x;   this->array[8] += this->array[11]* x;   this->array[12]+= this->array[15]* x;
    this->array[1] += this->array[3] * y;   this->array[5] += this->array[7] * y;   this->array[9] += this->array[11]* y;   this->array[13]+= this->array[15]* y;
    this->array[2] += this->array[3] * z;   this->array[6] += this->array[7] * z;   this->array[10]+= this->array[11]* z;   this->array[14]+= this->array[15]* z;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::scale(const Vector3& s)
{
    auto x = s.x, y = s.y, z = s.z;
    this->array[0] *= x;   this->array[4] *= x;   this->array[8] *= x;   this->array[12] *= x;
    this->array[1] *= y;   this->array[5] *= y;   this->array[9] *= y;   this->array[13] *= y;
    this->array[2] *= z;   this->array[6] *= z;   this->array[10]*= z;   this->array[14] *= z;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::eulerRotate(const Vector3& r)
{
    type rm;
    rm.setEulerRotation(r);
    (*this) *= rm;
}


//-----------------------------------------------------------------------------
template <typename T>
float Matrix44Base<T>::determinent()
{
    T inv[4];
    
    inv[0] = this->array[5]  * this->array[10] * this->array[15] -
    this->array[5]  * this->array[11] * this->array[14] -
    this->array[9]  * this->array[6]  * this->array[15] +
    this->array[9]  * this->array[7]  * this->array[14] +
    this->array[13] * this->array[6]  * this->array[11] -
    this->array[13] * this->array[7]  * this->array[10];
    
    inv[1] = -this->array[4]  * this->array[10] * this->array[15] +
    this->array[4]  * this->array[11] * this->array[14] +
    this->array[8]  * this->array[6]  * this->array[15] -
    this->array[8]  * this->array[7]  * this->array[14] -
    this->array[12] * this->array[6]  * this->array[11] +
    this->array[12] * this->array[7]  * this->array[10];
    
    inv[2] = this->array[4]  * this->array[9] * this->array[15] -
    this->array[4]  * this->array[11] * this->array[13] -
    this->array[8]  * this->array[5] * this->array[15] +
    this->array[8]  * this->array[7] * this->array[13] +
    this->array[12] * this->array[5] * this->array[11] -
    this->array[12] * this->array[7] * this->array[9];
    
    inv[3] = -this->array[4]  * this->array[9] * this->array[14] +
    this->array[4]  * this->array[10] * this->array[13] +
    this->array[8]  * this->array[5] * this->array[14] -
    this->array[8]  * this->array[6] * this->array[13] -
    this->array[12] * this->array[5] * this->array[10] +
    this->array[12] * this->array[6] * this->array[9];
    
    T det = this->array[0] * inv[0] + this->array[1] * inv[1]
    + this->array[2] * inv[2] + this->array[3] * inv[3];
    
    return det;
}


//-----------------------------------------------------------------------------
template <typename T>
bool Matrix44Base<T>::inverse()
{
    return this->inverseTo(*this);
}
    

//-----------------------------------------------------------------------------
template <typename T>
bool Matrix44Base<T>::inverseTo(type& out)
{
    T inv[16];

    inv[0] = this->array[5] * this->array[10] * this->array[15] -
        this->array[5] * this->array[11] * this->array[14] -
        this->array[9] * this->array[6] * this->array[15] +
        this->array[9] * this->array[7] * this->array[14] +
        this->array[13] * this->array[6] * this->array[11] -
        this->array[13] * this->array[7] * this->array[10];

    inv[4] = -this->array[4] * this->array[10] * this->array[15] +
        this->array[4] * this->array[11] * this->array[14] +
        this->array[8] * this->array[6] * this->array[15] -
        this->array[8] * this->array[7] * this->array[14] -
        this->array[12] * this->array[6] * this->array[11] +
        this->array[12] * this->array[7] * this->array[10];

    inv[8] = this->array[4] * this->array[9] * this->array[15] -
        this->array[4] * this->array[11] * this->array[13] -
        this->array[8] * this->array[5] * this->array[15] +
        this->array[8] * this->array[7] * this->array[13] +
        this->array[12] * this->array[5] * this->array[11] -
        this->array[12] * this->array[7] * this->array[9];

    inv[12] = -this->array[4] * this->array[9] * this->array[14] +
        this->array[4] * this->array[10] * this->array[13] +
        this->array[8] * this->array[5] * this->array[14] -
        this->array[8] * this->array[6] * this->array[13] -
        this->array[12] * this->array[5] * this->array[10] +
        this->array[12] * this->array[6] * this->array[9];

    inv[1] = -this->array[1] * this->array[10] * this->array[15] +
        this->array[1] * this->array[11] * this->array[14] +
        this->array[9] * this->array[2] * this->array[15] -
        this->array[9] * this->array[3] * this->array[14] -
        this->array[13] * this->array[2] * this->array[11] +
        this->array[13] * this->array[3] * this->array[10];

    inv[5] = this->array[0] * this->array[10] * this->array[15] -
        this->array[0] * this->array[11] * this->array[14] -
        this->array[8] * this->array[2] * this->array[15] +
        this->array[8] * this->array[3] * this->array[14] +
        this->array[12] * this->array[2] * this->array[11] -
        this->array[12] * this->array[3] * this->array[10];

    inv[9] = -this->array[0] * this->array[9] * this->array[15] +
        this->array[0] * this->array[11] * this->array[13] +
        this->array[8] * this->array[1] * this->array[15] -
        this->array[8] * this->array[3] * this->array[13] -
        this->array[12] * this->array[1] * this->array[11] +
        this->array[12] * this->array[3] * this->array[9];

    inv[13] = this->array[0] * this->array[9] * this->array[14] -
        this->array[0] * this->array[10] * this->array[13] -
        this->array[8] * this->array[1] * this->array[14] +
        this->array[8] * this->array[2] * this->array[13] +
        this->array[12] * this->array[1] * this->array[10] -
        this->array[12] * this->array[2] * this->array[9];

    inv[2] = this->array[1] * this->array[6] * this->array[15] -
        this->array[1] * this->array[7] * this->array[14] -
        this->array[5] * this->array[2] * this->array[15] +
        this->array[5] * this->array[3] * this->array[14] +
        this->array[13] * this->array[2] * this->array[7] -
        this->array[13] * this->array[3] * this->array[6];

    inv[6] = -this->array[0] * this->array[6] * this->array[15] +
        this->array[0] * this->array[7] * this->array[14] +
        this->array[4] * this->array[2] * this->array[15] -
        this->array[4] * this->array[3] * this->array[14] -
        this->array[12] * this->array[2] * this->array[7] +
        this->array[12] * this->array[3] * this->array[6];

    inv[10] = this->array[0] * this->array[5] * this->array[15] -
        this->array[0] * this->array[7] * this->array[13] -
        this->array[4] * this->array[1] * this->array[15] +
        this->array[4] * this->array[3] * this->array[13] +
        this->array[12] * this->array[1] * this->array[7] -
        this->array[12] * this->array[3] * this->array[5];

    inv[14] = -this->array[0] * this->array[5] * this->array[14] +
        this->array[0] * this->array[6] * this->array[13] +
        this->array[4] * this->array[1] * this->array[14] -
        this->array[4] * this->array[2] * this->array[13] -
        this->array[12] * this->array[1] * this->array[6] +
        this->array[12] * this->array[2] * this->array[5];

    inv[3] = -this->array[1] * this->array[6] * this->array[11] +
        this->array[1] * this->array[7] * this->array[10] +
        this->array[5] * this->array[2] * this->array[11] -
        this->array[5] * this->array[3] * this->array[10] -
        this->array[9] * this->array[2] * this->array[7] +
        this->array[9] * this->array[3] * this->array[6];

    inv[7] = this->array[0] * this->array[6] * this->array[11] -
        this->array[0] * this->array[7] * this->array[10] -
        this->array[4] * this->array[2] * this->array[11] +
        this->array[4] * this->array[3] * this->array[10] +
        this->array[8] * this->array[2] * this->array[7] -
        this->array[8] * this->array[3] * this->array[6];

    inv[11] = -this->array[0] * this->array[5] * this->array[11] +
        this->array[0] * this->array[7] * this->array[9] +
        this->array[4] * this->array[1] * this->array[11] -
        this->array[4] * this->array[3] * this->array[9] -
        this->array[8] * this->array[1] * this->array[7] +
        this->array[8] * this->array[3] * this->array[5];

    inv[15] = this->array[0] * this->array[5] * this->array[10] -
        this->array[0] * this->array[6] * this->array[9] -
        this->array[4] * this->array[1] * this->array[10] +
        this->array[4] * this->array[2] * this->array[9] +
        this->array[8] * this->array[1] * this->array[6] -
        this->array[8] * this->array[2] * this->array[5];

    T det = this->array[0] * inv[0] + this->array[1] * inv[4]
        + this->array[2] * inv[8] + this->array[3] * inv[12];
    if (det == 0) TOD_RETURN_TRACE(false);

    det = static_cast<T>(1.0) / det;

    for (int32 i = 0; i < 16; i++)
    {
        out.array[i] = inv[i] * det;
    }

    return true;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::transpose()
{
    std::swap(this->array[1], this->array[4]);
    std::swap(this->array[2], this->array[8]);
    std::swap(this->array[3], this->array[12]);
    std::swap(this->array[6], this->array[9]);
    std::swap(this->array[7], this->array[13]);
    std::swap(this->array[11], this->array[14]);
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::makeFromSRT(const Vector3& s, const Quaternion& r, const Vector3& t)
{
    auto rx = r.x;
    auto ry = r.y;
    auto rz = r.z;
    auto rw = r.w;
    auto sx = s.x;
    auto sy = s.y;
    auto sz = s.z;

    this->m11 = (1.0f - 2.0f * (ry * ry + rz * rz)) * sx;
    this->m12 = (2.0f * (rx * ry + rz * rw)) * sx;
    this->m13 = (2.0f * (rx * rz - ry * rw)) * sx;
    this->m14 = 0.0f;

    this->m21 = (2.0f * (rx * ry - rz * rw)) * sy;
    this->m22 = (1.0f - 2.0f * (rx * rx + rz * rz)) * sy;
    this->m23 = (2.0f * (ry * rz + rx * rw)) * sy;
    this->m24 = 0.0f;

    this->m31 = (2.0f * (rx * rz + ry * rw)) * sz;
    this->m32 = (2.0f * (ry * rz - rx * rw)) * sz;
    this->m33 = (1.0f - 2.0f * (rx * rx + ry * ry)) * sz;
    this->m34 = 0.0f;

    this->m41 = t.x;
    this->m42 = t.y;
    this->m43 = t.z;
    this->m44 = 1.0f;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::makeFromSRTTranspose
(const Vector3& s, const Quaternion& r, const Vector3& t)
{
    auto rx = r.x;
    auto ry = r.y;
    auto rz = r.z;
    auto rw = r.w;
    auto sx = s.x;
    auto sy = s.y;
    auto sz = s.z;

    this->m11 = (1.0f - 2.0f * (ry * ry + rz * rz)) * sx;
    this->m21 = (2.0f * (rx * ry + rz * rw)) * sx;
    this->m31 = (2.0f * (rx * rz - ry * rw)) * sx;
    this->m41 = 0.0f;

    this->m12 = (2.0f * (rx * ry - rz * rw)) * sy;
    this->m22 = (1.0f - 2.0f * (rx * rx + rz * rz)) * sy;
    this->m32 = (2.0f * (ry * rz + rx * rw)) * sy;
    this->m42 = 0.0f;

    this->m13 = (2.0f * (rx * rz + ry * rw)) * sz;
    this->m23 = (2.0f * (ry * rz - rx * rw)) * sz;
    this->m33 = (1.0f - 2.0f * (rx * rx + ry * ry)) * sz;
    this->m43 = 0.0f;

    this->m14 = t.x;
    this->m24 = t.y;
    this->m34 = t.z;
    this->m44 = 1.0f;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::makeFromQuaternion(const Quaternion& r)
{
    auto x = r.x;
    auto y = r.y;
    auto z = r.z;
    auto w = r.w;

    this->m11 = 1.0f - 2.0f * (y * y + z * z);
    this->m12 = 2.0f * (x * y + z * w);
    this->m13 = 2.0f * (x * z - y * w);
    this->m14 = 0.0f;

    this->m21 = 2.0f * (x * y - z * w);
    this->m22 = 1.0f - 2.0f * (x * x + z * z);
    this->m23 = 2.0f * (y * z + x * w);
    this->m24 = 0.0f;

    this->m31 = 2.0f * (x * z + y * w);
    this->m32 = 2.0f * (y * z - x * w);
    this->m33 = 1.0f - 2.0f * (x * x + y * y);
    this->m34 = 0.0f;

    this->m41 = 0.0f;
    this->m42 = 0.0f;
    this->m43 = 0.0f;
    this->m44 = 1.0f;
}


//-----------------------------------------------------------------------------
template <typename T>
Vector3 Matrix44Base<T>::getRightVector()
{
    return Vector3(this->m11, this->m12, this->m13);
}


//-----------------------------------------------------------------------------
template <typename T>
Vector3 Matrix44Base<T>::getUpVector()
{
    return Vector3(this->m21, this->m22, this->m23);
}


//-----------------------------------------------------------------------------
template <typename T>
Vector3 Matrix44Base<T>::getForwardVector()
{
    return Vector3(this->m31, this->m32, this->m33);
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::lookAt(const Vector3& target)
{
    Vector3 position { this->getTranslation() };
    Vector3 forward { target };
    forward -= position;
    forward.normalize();
    Vector3 up;
    Vector3 left;
    
    if(Math::abs(forward.x) < Math::EPSILON && Math::abs(forward.z) < Math::EPSILON)
    {
        if(forward.y > 0)
            up = { 0, 0, -1 };
        else
            up = { 0, 0, 1 };
    }
    else
    {
        up = { 0, 1, 0 };
    }
    
    left = up.cross(forward);
    left.normalize();
    up = forward.cross(left);
    
    this->setColumn(0, left);
    this->setColumn(1, up);
    this->setColumn(2, forward);
}
    

//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::lookAt(const Vector3& target, const Vector3& up)
{
    Vector3 position { this->getTranslation() };
    Vector3 forward { target };
    forward -= position;
    forward.normalize();
    
    Vector3 left = up.cross(forward);
    left.normalize();
    
    Vector3 up_vec { forward.cross(left) };
    up_vec.normalize();
    
    this->m11 = left.x;
    this->m21 = left.y;
    this->m31 = left.z;
              =
    this->m12 = up_vec.x;
    this->m22 = up_vec.y;
    this->m32 = up_vec.z;
              =
    this->m13 = forward.x;
    this->m23 = forward.y;
    this->m33 = forward.z;
}


//-----------------------------------------------------------------------------
template <typename T>
Vector3 Matrix44Base<T>::getAngle()
{
    float yaw = Math::rad2deg(Math::asin(this->array[8]));
    if(this->array[10] < 0)
    {
        if(yaw >= 0) yaw = 180.0f - yaw;
        else         yaw =-180.0f - yaw;
    }
    
    float roll = 0;
    float pitch = 0;
    if(this->array[0] > -Math::EPSILON && this->array[0] < Math::EPSILON)
    {
        pitch = Math::rad2deg(Math::atan2(this->array[1], this->array[5]));
    }
    else
    {
        roll = Math::rad2deg(Math::atan2(-this->array[4], this->array[0]));
        pitch = Math::rad2deg(Math::atan2(-this->array[9], this->array[10]));
    }
    
    return Vector3(pitch, yaw, roll);
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::perspectiveLH(T fov, T aspect, T zNear, T zFar)
{
    this->clear();
    
    T a = 1 / (Math::tan(Math::deg2rad(fov) / 2));
    this->m11 = a / aspect;
    this->m22 = a;
    this->m34 = static_cast<T>(1);
    
    //OpenGL NDC

    //DirectX NDC
    T Q = zFar / (zFar - zNear);
    this->m33 = Q;
    this->m43 = -Q * zNear;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::perspectiveRH(T fov, T aspect, T zNear, T zFar)
{
    this->clear();
    
    auto tanHalfFov = Math::tan(Math::deg2rad(fov) / static_cast<T>(2));
    this->m11 = static_cast<T>(1) / (aspect * tanHalfFov);
    this->m22 = static_cast<T>(1) / (tanHalfFov);
    this->m34 = static_cast<T>(-1);
    
    //OpenGL NDC (Normalized Device Coodinate) : depth -1 ~ 1
    this->m33 = - (zFar + zNear) / (zFar - zNear);
    this->m43 = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
    
    //DirectX NDC : detph 0 ~ 1
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::orthogonalLH
(T left, T right, T bottom, T top, T zNear, T zFar)
{
    this->clear();

    //미구현
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::orthogonalRH
(T left, T right, T bottom, T top, T zNear, T zFar)
{
    this->identity();

    this->m11 = static_cast<T>(2) / (right - left);
    this->m22 = static_cast<T>(2) / (top - bottom);
    this->m41 = - (right + left) / (right - left);
    this->m42 = - (top + bottom) / (top - bottom);

    this->m33 = - static_cast<T>(2) / (zFar - zNear);
    this->m43 = - (zFar + zNear) / (zFar - zNear);
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::setX(T value)
{
    this->m41 = value;
}


//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::setY(T value)
{
    this->m42 = value;
}



//-----------------------------------------------------------------------------
template <typename T>
void Matrix44Base<T>::setZ(T value)
{
    this->m43 = value;
}



//-----------------------------------------------------------------------------
template <typename T>
T Matrix44Base<T>::getX()
{
    return this->m41;
}



//-----------------------------------------------------------------------------
template <typename T>
T Matrix44Base<T>::getY()
{
    return this->m42;
}



//-----------------------------------------------------------------------------
template <typename T>
T Matrix44Base<T>::getZ()
{
    return this->m43;
}


//-----------------------------------------------------------------------------
template <typename T>
T& Matrix44Base<T>::m(int32 row, int32 column)
{
    return this->d[row][column];
}


//-----------------------------------------------------------------------------
template <typename T>
const T& Matrix44Base<T>::m(int32 row, int32 column) const
{
    return this->d[row][column];
}


//-----------------------------------------------------------------------------
template <typename T>
typename Matrix44Base<T>::type& Matrix44Base<T>::operator *= (const type& other)
{
    type output;
    output.clear();
    for(int32 i = 0; i < 4; ++i)
    {
        for(int32 j = 0; j < 4; ++j)
        {
            for(int32 k = 0; k < 4; ++k)
            {
                output.d[i][j] += this->d[i][k] * other.d[k][j];
            }
        }
    }
    return (*this = output);
}


//-----------------------------------------------------------------------------
template <typename T>
typename Matrix44Base<T>::type& Matrix44Base<T>::operator += (const type& other)
{
    for(int32 i = 0; i < 16; ++i)
    {
        this->array[i] += other.array[i];
    }
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Matrix44Base<T>::type& Matrix44Base<T>::operator -= (const type& other)
{
    for(int32 i = 0; i < 16; ++i)
    {
        this->array[i] -= other.array[i];
    }
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T>
typename Matrix44Base<T>::type
Matrix44Base<T>::operator * (const type& other) const
{
    auto ret(*this);
    ret *= other;
    return ret;
}


//-----------------------------------------------------------------------------
template <typename T>
Matrix44Base<T>::operator float* ()
{
    return this->array;
}


//-----------------------------------------------------------------------------
#if defined (TOD_PLATFORM_WINDOWS)
typedef Matrix44Base<float> Matrix44;
#else
typedef Matrix44Base<float> Matrix44;
#endif

    
}
