#pragma once
#include "tod/graphics/vector3.h"
namespace tod::graphics
{
    
//-----------------------------------------------------------------------------
class MatrixRowMajor
{
public:
    template <typename T>
    static inline void setElement(T* array, int32 row, int32 column, T value)
    {
        array[(row << 2) + column] = value;
    }
    template <typename T>
    static inline T getElement(T* array, int32 row, int32 column)
    {
        return array[(row << 2) + column];
    }
    template <typename T>
    static inline void add(T* array, int32 row, int32 column, T value)
    {
        array[(row << 2) + column] += value;
    }
};


//-----------------------------------------------------------------------------
class MatrixColumnMajor
{
public:
    template <typename T>
    static inline void setElement(T* array, int32 row, int32 column, T value)
    {
        array[(row << 2) + column] = value;
    }
    template <typename T>
    static inline T getElement(T* array, int32 row, int32 column)
    {
        return array[(row << 2) + column];
    }
    template <typename T>
    static inline void add(T* array, int32 row, int32 column, T value)
    {
        array[(row << 2) + column] += value;
    }
};


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR=MatrixColumnMajor>
class Matrix33Base
{
public:
    typedef Matrix33Base<T, MAJOR> type;
    
public:
    Matrix33Base();
    
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
    void transpose();
    Vector3 getRightVector();
    Vector3 getUpVector();
    Vector3 getForwardVector();
    
    void lookAt(const Vector3& target);
    void lookAt(const Vector3& target, const Vector3& up);
    Vector3 getAngle();    
    
    void setX(T value);
    void setY(T value);
    void setZ(T value);
    T getX();
    T getY();
    T getZ();
    
    type& operator += (const type& other);
    type& operator -= (const type& other);
    type& operator *= (const type& other);

    operator float* ();
    
public:
    union
    {
        T d[3][3];
        T array[9];
    };
};


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
Matrix33Base<T, MAJOR>::Matrix33Base()
{
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::clear()
{
    memset(this->d, 0, sizeof(type));
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::identity()
{
    this->clear();
    for (int32 i=0;i<4;++i) d[i][i] = 1;
}
    

//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::setTranslation(const Vector3& t)
{
    this->identity();
    MAJOR::setElement(this->array, 3, 0, t.x);
    MAJOR::setElement(this->array, 3, 1, t.y);
    MAJOR::setElement(this->array, 3, 2, t.z);
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
Vector3 Matrix33Base<T, MAJOR>::getTranslation() const
{
    return Vector3(
        MAJOR::getElement(this->array, 3, 0),
        MAJOR::getElement(this->array, 3, 1),
        MAJOR::getElement(this->array, 3, 2));
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::setEulerRotation(const Vector3& r)
{
    this->clear();
    auto sin_rx = Math::sin(r.x);
    auto cos_rx = Math::cos(r.x);
    
    auto sin_ry = Math::sin(r.y);
    auto cos_ry = Math::cos(r.y);
    
    auto sin_rz = Math::sin(r.z);
    auto cos_rz = Math::cos(r.z);
    
    MAJOR::setElement(this->array, 0, 0, cos_ry * cos_rz);
    MAJOR::setElement(this->array, 0, 1, -cos_ry * sin_rz);
    MAJOR::setElement(this->array, 0, 2, sin_ry);
    
    MAJOR::setElement(this->array, 1, 0, cos_rx * sin_rz + sin_rx * sin_ry * cos_rz);
    MAJOR::setElement(this->array, 1, 1, cos_rx * cos_rz - sin_rx * sin_ry * sin_rz);
    MAJOR::setElement(this->array, 1, 2, -sin_rx * cos_ry);
    
    MAJOR::setElement(this->array, 2, 0, sin_rx * sin_rz - cos_rx * sin_ry * cos_rz);
    MAJOR::setElement(this->array, 2, 1, sin_rx * cos_rz + cos_rx * sin_ry * sin_rz);
    MAJOR::setElement(this->array, 2, 2, cos_rx * cos_ry);
    
    MAJOR::setElement(this->array, 3, 3, 1.0f);
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::setScaling(const Vector3& s)
{
    this->clear();
    MAJOR::setElement(this->array, 0, 0, s.x);
    MAJOR::setElement(this->array, 1, 1, s.y);
    MAJOR::setElement(this->array, 2, 2, s.z);
    MAJOR::setElement(this->array, 3, 3, 1.0f);
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::translate(const Vector3& t)
{
    auto x = t.x, y = t.y, z = t.z;
    this->array[0] += this->array[3] * x;   this->array[4] += this->array[7] * x;   this->array[8] += this->array[11]* x;   this->array[12]+= this->array[15]* x;
    this->array[1] += this->array[3] * y;   this->array[5] += this->array[7] * y;   this->array[9] += this->array[11]* y;   this->array[13]+= this->array[15]* y;
    this->array[2] += this->array[3] * z;   this->array[6] += this->array[7] * z;   this->array[10]+= this->array[11]* z;   this->array[14]+= this->array[15]* z;
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::scale(const Vector3& s)
{
    auto x = s.x, y = s.y, z = s.z;
    this->array[0] *= x;   this->array[4] *= x;   this->array[8] *= x;   this->array[12] *= x;
    this->array[1] *= y;   this->array[5] *= y;   this->array[9] *= y;   this->array[13] *= y;
    this->array[2] *= z;   this->array[6] *= z;   this->array[10]*= z;   this->array[14] *= z;
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::eulerRotate(const Vector3& r)
{
    type rm;
    rm.setEulerRotation(r);
    (*this) *= rm;
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
float Matrix33Base<T, MAJOR>::determinent()
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
template <typename T, typename MAJOR>
bool Matrix33Base<T, MAJOR>::inverse()
{
    T inv[16];
    
    inv[0] = this->array[5]  * this->array[10] * this->array[15] -
    this->array[5]  * this->array[11] * this->array[14] -
    this->array[9]  * this->array[6]  * this->array[15] +
    this->array[9]  * this->array[7]  * this->array[14] +
    this->array[13] * this->array[6]  * this->array[11] -
    this->array[13] * this->array[7]  * this->array[10];
    
    inv[4] = -this->array[4]  * this->array[10] * this->array[15] +
    this->array[4]  * this->array[11] * this->array[14] +
    this->array[8]  * this->array[6]  * this->array[15] -
    this->array[8]  * this->array[7]  * this->array[14] -
    this->array[12] * this->array[6]  * this->array[11] +
    this->array[12] * this->array[7]  * this->array[10];
    
    inv[8] = this->array[4]  * this->array[9] * this->array[15] -
    this->array[4]  * this->array[11] * this->array[13] -
    this->array[8]  * this->array[5] * this->array[15] +
    this->array[8]  * this->array[7] * this->array[13] +
    this->array[12] * this->array[5] * this->array[11] -
    this->array[12] * this->array[7] * this->array[9];
    
    inv[12] = -this->array[4]  * this->array[9] * this->array[14] +
    this->array[4]  * this->array[10] * this->array[13] +
    this->array[8]  * this->array[5] * this->array[14] -
    this->array[8]  * this->array[6] * this->array[13] -
    this->array[12] * this->array[5] * this->array[10] +
    this->array[12] * this->array[6] * this->array[9];
    
    inv[1] = -this->array[1]  * this->array[10] * this->array[15] +
    this->array[1]  * this->array[11] * this->array[14] +
    this->array[9]  * this->array[2] * this->array[15] -
    this->array[9]  * this->array[3] * this->array[14] -
    this->array[13] * this->array[2] * this->array[11] +
    this->array[13] * this->array[3] * this->array[10];
    
    inv[5] = this->array[0]  * this->array[10] * this->array[15] -
    this->array[0]  * this->array[11] * this->array[14] -
    this->array[8]  * this->array[2] * this->array[15] +
    this->array[8]  * this->array[3] * this->array[14] +
    this->array[12] * this->array[2] * this->array[11] -
    this->array[12] * this->array[3] * this->array[10];
    
    inv[9] = -this->array[0]  * this->array[9] * this->array[15] +
    this->array[0]  * this->array[11] * this->array[13] +
    this->array[8]  * this->array[1] * this->array[15] -
    this->array[8]  * this->array[3] * this->array[13] -
    this->array[12] * this->array[1] * this->array[11] +
    this->array[12] * this->array[3] * this->array[9];
    
    inv[13] = this->array[0]  * this->array[9] * this->array[14] -
    this->array[0]  * this->array[10] * this->array[13] -
    this->array[8]  * this->array[1] * this->array[14] +
    this->array[8]  * this->array[2] * this->array[13] +
    this->array[12] * this->array[1] * this->array[10] -
    this->array[12] * this->array[2] * this->array[9];
    
    inv[2] = this->array[1]  * this->array[6] * this->array[15] -
    this->array[1]  * this->array[7] * this->array[14] -
    this->array[5]  * this->array[2] * this->array[15] +
    this->array[5]  * this->array[3] * this->array[14] +
    this->array[13] * this->array[2] * this->array[7] -
    this->array[13] * this->array[3] * this->array[6];
    
    inv[6] = -this->array[0]  * this->array[6] * this->array[15] +
    this->array[0]  * this->array[7] * this->array[14] +
    this->array[4]  * this->array[2] * this->array[15] -
    this->array[4]  * this->array[3] * this->array[14] -
    this->array[12] * this->array[2] * this->array[7] +
    this->array[12] * this->array[3] * this->array[6];
    
    inv[10] = this->array[0]  * this->array[5] * this->array[15] -
    this->array[0]  * this->array[7] * this->array[13] -
    this->array[4]  * this->array[1] * this->array[15] +
    this->array[4]  * this->array[3] * this->array[13] +
    this->array[12] * this->array[1] * this->array[7] -
    this->array[12] * this->array[3] * this->array[5];
    
    inv[14] = -this->array[0]  * this->array[5] * this->array[14] +
    this->array[0]  * this->array[6] * this->array[13] +
    this->array[4]  * this->array[1] * this->array[14] -
    this->array[4]  * this->array[2] * this->array[13] -
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
        this->array[i] = inv[i] * det;
    
    return true;
}
    

//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::transpose()
{
    std::swap(this->array[1], this->array[4]);
    std::swap(this->array[2], this->array[8]);
    std::swap(this->array[3], this->array[12]);
    std::swap(this->array[6], this->array[9]);
    std::swap(this->array[7], this->array[13]);
    std::swap(this->array[11], this->array[14]);
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
Vector3 Matrix33Base<T, MAJOR>::getRightVector()
{
    return Vector3(
        MAJOR::getElement(this->array, 0, 0),
        MAJOR::getElement(this->array, 0, 1),
        MAJOR::getElement(this->array, 0, 2));
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
Vector3 Matrix33Base<T, MAJOR>::getUpVector()
{
    return Vector3(
        MAJOR::getElement(this->array, 1, 0),
        MAJOR::getElement(this->array, 1, 1),
        MAJOR::getElement(this->array, 1, 2));
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
Vector3 Matrix33Base<T, MAJOR>::getForwardVector()
{
    return Vector3(
        MAJOR::getElement(this->array, 2, 0),
        MAJOR::getElement(this->array, 2, 1),
        MAJOR::getElement(this->array, 2, 2));
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::lookAt(const Vector3& target)
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
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::lookAt(const Vector3& target, const Vector3& up)
{
    Vector3 position { this->getTranslation() };
    Vector3 forward { target };
    forward -= position;
    forward.normalize();
    
    Vector3 left = up.cross(forward);
    left.normalize();
    
    Vector3 up_vec { forward.cross(left) };
    up_vec.normalize();
    
    MAJOR::setElement(this->array, 0, 0, left.x);
    MAJOR::setElement(this->array, 1, 0, left.y);
    MAJOR::setElement(this->array, 2, 0, left.z);
    
    MAJOR::setElement(this->array, 0, 1, up_vec.x);
    MAJOR::setElement(this->array, 1, 1, up_vec.y);
    MAJOR::setElement(this->array, 2, 1, up_vec.z);

    MAJOR::setElement(this->array, 0, 2, forward.x);
    MAJOR::setElement(this->array, 1, 2, forward.y);
    MAJOR::setElement(this->array, 2, 2, forward.z);
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
Vector3 Matrix33Base<T, MAJOR>::getAngle()
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
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::perspectiveLH(T fov, T aspect, T zNear, T zFar)
{
    this->clear();
    
    T a = 1 / (Math::tan(Math::deg2rad(fov) / 2));
    MAJOR::setElement(this->array, 0, 0, a / aspect);
    MAJOR::setElement(this->array, 1, 1, a);
    MAJOR::setElement(this->array, 2, 3, static_cast<T>(1));
    
    //OpenGL NDC

    //DirectX NDC
    T Q = zFar / (zFar - zNear);
    MAJOR::setElement(this->array, 2, 2, Q);
    MAJOR::setElement(this->array, 3, 2, -Q * zNear);    
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::perspectiveRH(T fov, T aspect, T zNear, T zFar)
{
    this->clear();
    
    auto tanHalfFov = Math::tan(Math::deg2rad(fov) / static_cast<T>(2));
    MAJOR::setElement(this->array, 0, 0, static_cast<T>(1) / (aspect * tanHalfFov));
    MAJOR::setElement(this->array, 1, 1, static_cast<T>(1) / (tanHalfFov));
    MAJOR::setElement(this->array, 2, 3, static_cast<T>(-1));
    
    //OpenGL NDC (Normalized Device Coodinate) : depth -1 ~ 1
    MAJOR::setElement(this->array, 2, 2, - (zFar + zNear) / (zFar - zNear));
    MAJOR::setElement(this->array, 3, 2, - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear));
    
    //DirectX NDC : detph 0 ~ 1
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::orthogonalLH
(T left, T right, T bottom, T top, T zNear, T zFar)
{
    this->clear();

    //¹Ì±¸Çö
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::orthogonalRH
(T left, T right, T bottom, T top, T zNear, T zFar)
{
    this->identity();

    MAJOR::setElement(this->array, 0, 0, static_cast<T>(2) / (right - left));
    MAJOR::setElement(this->array, 1, 1, static_cast<T>(2) / (top - bottom));
    MAJOR::setElement(this->array, 3, 0, - (right + left) / (right - left));
    MAJOR::setElement(this->array, 3, 1, - (top + bottom) / (top - bottom));

    MAJOR::setElement(this->array, 2, 2, - static_cast<T>(2) / (zFar - zNear));
    MAJOR::setElement(this->array, 3, 2, - (zFar + zNear) / (zFar - zNear));
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::setX(T value)
{
    MAJOR::setElement(this->array, 3, 0, value);
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::setY(T value)
{
    MAJOR::setElement(this->array, 3, 1, value);
}



//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
void Matrix33Base<T, MAJOR>::setZ(T value)
{
    MAJOR::setElement(this->array, 3, 2, value);
}



//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
T Matrix33Base<T, MAJOR>::getX()
{
    return MAJOR::getElement(this->array, 3, 0);
}



//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
T Matrix33Base<T, MAJOR>::getY()
{
    return MAJOR::getElement(this->array, 3, 1);
}



//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
T Matrix33Base<T, MAJOR>::getZ()
{
    return MAJOR::getElement(this->array, 3, 2);
}



//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
typename Matrix33Base<T, MAJOR>::type& Matrix33Base<T, MAJOR>::operator *= (const type& other)
{
    type output;
    output.clear();
    for(int32 i = 0; i < 4; ++i)
    {
        for(int32 j = 0; j < 4; ++j)
        {
            for(int32 k = 0; k < 4; ++k)
            {
                MAJOR::add(output.array, i, j,
                    MAJOR::getElement(this->array, i, k) *
                    MAJOR::getElement(other.array, k, j));
            }
        }
    }
    return (*this = output);
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
typename Matrix33Base<T, MAJOR>::type& Matrix33Base<T, MAJOR>::operator += (const type& other)
{
    for(int32 i = 0; i < 16; ++i)
    {
        this->array[i] += other.array[i];
    }
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
typename Matrix33Base<T, MAJOR>::type& Matrix33Base<T, MAJOR>::operator -= (const type& other)
{
    for(int32 i = 0; i < 16; ++i)
    {
        this->array[i] -= other.array[i];
    }
    return *this;
}


//-----------------------------------------------------------------------------
template <typename T, typename MAJOR>
Matrix33Base<T, MAJOR>::operator float* ()
{
    return this->array;
}


//-----------------------------------------------------------------------------
#if defined (PLATFORM_WINDOWS)
typedef Matrix33Base<float, MatrixRowMajor> Matrix33;
#else
typedef Matrix33Base<float> Matrix33;
#endif

    
}
