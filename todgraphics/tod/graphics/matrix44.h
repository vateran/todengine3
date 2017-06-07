#pragma once
#include "tod/graphics/vector3.h"
namespace tod::graphics
{
    
template <typename T>
class Matrix44Base
{
public:
    typedef Matrix44Base<T> type;
    
public:
    Matrix44Base();
    
    void clear();
    void identity();
    void setTranslation(const Vector3& t);
    void setEulerRotation(const Vector3& r);
    void setScaling(const Vector3& s);
    float determinent();
    bool inverse();
    void transpose();
    
    void lookAt(const Vector3& target);
    void lookAt(const Vector3& target, const Vector3& up);
    Vector3 getAngle();
    
    type& operator += (const type& other);
    type& operator -= (const type& other);
    type& operator *= (const type& other);
    
public:
    union
    {
        T d[4][4];
        T array[16];
    };
};

    
template <typename T>
Matrix44Base<T>::Matrix44Base()
{
    this->clear();
}


template <typename T>
void Matrix44Base<T>::clear()
{
    memset(this->d, 0, sizeof(type));
}


template <typename T>
void Matrix44Base<T>::identity()
{
    this->clear();
    for (int i=0;i<4;++i) d[i][i] = 1;
}
    


template <typename T>
void Matrix44Base<T>::setTranslation(const Vector3& t)
{
    d[3][0] = t.x;
    d[3][1] = t.y;
    d[3][2] = t.z;
}

    
template <typename T>
void Matrix44Base<T>::setEulerRotation(const Vector3& r)
{
    
}


template <typename T>
void Matrix44Base<T>::setScaling(const Vector3& s)
{
    
}


template <typename T>
float Matrix44Base<T>::determinent()
{
    T inv[4];
    
    inv[0] = this->d[5]  * this->d[10] * this->d[15] -
    this->d[5]  * this->d[11] * this->d[14] -
    this->d[9]  * this->d[6]  * this->d[15] +
    this->d[9]  * this->d[7]  * this->d[14] +
    this->d[13] * this->d[6]  * this->d[11] -
    this->d[13] * this->d[7]  * this->d[10];
    
    inv[1] = -this->d[4]  * this->d[10] * this->d[15] +
    this->d[4]  * this->d[11] * this->d[14] +
    this->d[8]  * this->d[6]  * this->d[15] -
    this->d[8]  * this->d[7]  * this->d[14] -
    this->d[12] * this->d[6]  * this->d[11] +
    this->d[12] * this->d[7]  * this->d[10];
    
    inv[2] = this->d[4]  * this->d[9] * this->d[15] -
    this->d[4]  * this->d[11] * this->d[13] -
    this->d[8]  * this->d[5] * this->d[15] +
    this->d[8]  * this->d[7] * this->d[13] +
    this->d[12] * this->d[5] * this->d[11] -
    this->d[12] * this->d[7] * this->d[9];
    
    inv[3] = -this->d[4]  * this->d[9] * this->d[14] +
    this->d[4]  * this->d[10] * this->d[13] +
    this->d[8]  * this->d[5] * this->d[14] -
    this->d[8]  * this->d[6] * this->d[13] -
    this->d[12] * this->d[5] * this->d[10] +
    this->d[12] * this->d[6] * this->d[9];
    
    T det = this->d[0] * inv[0] + this->d[1] * inv[1]
    + this->d[2] * inv[2] + this->d[3] * inv[3];
    
    return det;
}

    
template <typename T>
bool Matrix44Base<T>::inverse()
{
    T inv[16];
    
    inv[0] = this->d[5]  * this->d[10] * this->d[15] -
    this->d[5]  * this->d[11] * this->d[14] -
    this->d[9]  * this->d[6]  * this->d[15] +
    this->d[9]  * this->d[7]  * this->d[14] +
    this->d[13] * this->d[6]  * this->d[11] -
    this->d[13] * this->d[7]  * this->d[10];
    
    inv[4] = -this->d[4]  * this->d[10] * this->d[15] +
    this->d[4]  * this->d[11] * this->d[14] +
    this->d[8]  * this->d[6]  * this->d[15] -
    this->d[8]  * this->d[7]  * this->d[14] -
    this->d[12] * this->d[6]  * this->d[11] +
    this->d[12] * this->d[7]  * this->d[10];
    
    inv[8] = this->d[4]  * this->d[9] * this->d[15] -
    this->d[4]  * this->d[11] * this->d[13] -
    this->d[8]  * this->d[5] * this->d[15] +
    this->d[8]  * this->d[7] * this->d[13] +
    this->d[12] * this->d[5] * this->d[11] -
    this->d[12] * this->d[7] * this->d[9];
    
    inv[12] = -this->d[4]  * this->d[9] * this->d[14] +
    this->d[4]  * this->d[10] * this->d[13] +
    this->d[8]  * this->d[5] * this->d[14] -
    this->d[8]  * this->d[6] * this->d[13] -
    this->d[12] * this->d[5] * this->d[10] +
    this->d[12] * this->d[6] * this->d[9];
    
    inv[1] = -this->d[1]  * this->d[10] * this->d[15] +
    this->d[1]  * this->d[11] * this->d[14] +
    this->d[9]  * this->d[2] * this->d[15] -
    this->d[9]  * this->d[3] * this->d[14] -
    this->d[13] * this->d[2] * this->d[11] +
    this->d[13] * this->d[3] * this->d[10];
    
    inv[5] = this->d[0]  * this->d[10] * this->d[15] -
    this->d[0]  * this->d[11] * this->d[14] -
    this->d[8]  * this->d[2] * this->d[15] +
    this->d[8]  * this->d[3] * this->d[14] +
    this->d[12] * this->d[2] * this->d[11] -
    this->d[12] * this->d[3] * this->d[10];
    
    inv[9] = -this->d[0]  * this->d[9] * this->d[15] +
    this->d[0]  * this->d[11] * this->d[13] +
    this->d[8]  * this->d[1] * this->d[15] -
    this->d[8]  * this->d[3] * this->d[13] -
    this->d[12] * this->d[1] * this->d[11] +
    this->d[12] * this->d[3] * this->d[9];
    
    inv[13] = this->d[0]  * this->d[9] * this->d[14] -
    this->d[0]  * this->d[10] * this->d[13] -
    this->d[8]  * this->d[1] * this->d[14] +
    this->d[8]  * this->d[2] * this->d[13] +
    this->d[12] * this->d[1] * this->d[10] -
    this->d[12] * this->d[2] * this->d[9];
    
    inv[2] = this->d[1]  * this->d[6] * this->d[15] -
    this->d[1]  * this->d[7] * this->d[14] -
    this->d[5]  * this->d[2] * this->d[15] +
    this->d[5]  * this->d[3] * this->d[14] +
    this->d[13] * this->d[2] * this->d[7] -
    this->d[13] * this->d[3] * this->d[6];
    
    inv[6] = -this->d[0]  * this->d[6] * this->d[15] +
    this->d[0]  * this->d[7] * this->d[14] +
    this->d[4]  * this->d[2] * this->d[15] -
    this->d[4]  * this->d[3] * this->d[14] -
    this->d[12] * this->d[2] * this->d[7] +
    this->d[12] * this->d[3] * this->d[6];
    
    inv[10] = this->d[0]  * this->d[5] * this->d[15] -
    this->d[0]  * this->d[7] * this->d[13] -
    this->d[4]  * this->d[1] * this->d[15] +
    this->d[4]  * this->d[3] * this->d[13] +
    this->d[12] * this->d[1] * this->d[7] -
    this->d[12] * this->d[3] * this->d[5];
    
    inv[14] = -this->d[0]  * this->d[5] * this->d[14] +
    this->d[0]  * this->d[6] * this->d[13] +
    this->d[4]  * this->d[1] * this->d[14] -
    this->d[4]  * this->d[2] * this->d[13] -
    this->d[12] * this->d[1] * this->d[6] +
    this->d[12] * this->d[2] * this->d[5];
    
    inv[3] = -this->d[1] * this->d[6] * this->d[11] +
    this->d[1] * this->d[7] * this->d[10] +
    this->d[5] * this->d[2] * this->d[11] -
    this->d[5] * this->d[3] * this->d[10] -
    this->d[9] * this->d[2] * this->d[7] +
    this->d[9] * this->d[3] * this->d[6];
    
    inv[7] = this->d[0] * this->d[6] * this->d[11] -
    this->d[0] * this->d[7] * this->d[10] -
    this->d[4] * this->d[2] * this->d[11] +
    this->d[4] * this->d[3] * this->d[10] +
    this->d[8] * this->d[2] * this->d[7] -
    this->d[8] * this->d[3] * this->d[6];
    
    inv[11] = -this->d[0] * this->d[5] * this->d[11] +
    this->d[0] * this->d[7] * this->d[9] +
    this->d[4] * this->d[1] * this->d[11] -
    this->d[4] * this->d[3] * this->d[9] -
    this->d[8] * this->d[1] * this->d[7] +
    this->d[8] * this->d[3] * this->d[5];
    
    inv[15] = this->d[0] * this->d[5] * this->d[10] -
    this->d[0] * this->d[6] * this->d[9] -
    this->d[4] * this->d[1] * this->d[10] +
    this->d[4] * this->d[2] * this->d[9] +
    this->d[8] * this->d[1] * this->d[6] -
    this->d[8] * this->d[2] * this->d[5];
    
    T det = this->d[0] * inv[0] + this->d[1] * inv[4]
    + this->d[2] * inv[8] + this->d[3] * inv[12];
    if (det == 0)
        return false;
    
    det = 1.0 / det;
    
    for (int i = 0; i < 16; i++)
        this->array[i] = inv[i] * det;
    
    return true;
}
    

template <typename T>
void Matrix44Base<T>::transpose()
{
    std::swap(this->d[1], this->d[4]);
    std::swap(this->d[2], this->d[8]);
    std::swap(this->d[3], this->d[12]);
    std::swap(this->d[6], this->d[9]);
    std::swap(this->d[7], this->d[13]);
    std::swap(this->d[11], this->d[14]);
}
    

template <typename T>
void Matrix44Base<T>::lookAt(const Vector3& target)
{
    Vector3 position { this->getPosition() };
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
    

template <typename T>
void Matrix44Base<T>::lookAt(const Vector3& target, const Vector3& up)
{
    Vector3 position { this->getPosition() };
    Vector3 forward { target };
    forward -= position;
    forward.normalize();
    
    Vector3 left = up.cross(forward);
    left.normalize();
    
    Vector3 up_vec { forward.cross(left) };
    up_vec.normalize();
    
    this->setColumn(0, left);
    this->setColumn(1, up_vec);
    this->setColumn(2, forward);
}


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


template <typename T>
typename Matrix44Base<T>::type& Matrix44Base<T>::operator *= (const type& other)
{
    type output;
    output.clear();
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            for(int k=0; k<4; ++k)
            {
                output.d[i][j] += this->d[i][k] * other.d[k][j];
            }
        }
    }
    return (*this = output);
}


template <typename T>
typename Matrix44Base<T>::type& Matrix44Base<T>::operator += (const type& other)
{
    for(int i = 0; i < 16; ++i)
    {
        this->d[i] += other.array[i];
    }
    return *this;
}


template <typename T>
typename Matrix44Base<T>::type& Matrix44Base<T>::operator -= (const type& other)
{
    for(int i = 0; i < 16; ++i)
    {
        this->d[i] -= other.array[i];
    }
    return *this;
}
    
typedef Matrix44Base<float> Matrix44;

    
}
