#pragma once
namespace tod::graphics
{

template <typename T>
class PlaneBase
{
public:
    typedef typename Vector3Base<T> Vector3Type;

public:
    PlaneBase()
    : x(0)
    , y(0)
    , z(0)
    , w(0)
    {

    }
    PlaneBase(const T& x, const T& y, const T& z, const T& w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
    {

    }

    void set(const T& x, const T& y, const T& z, const T& w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    T dot(const Vector3Type& other) const
    {
        return this->normal.dot(other) + this->w;
    }

    void normalize()
    {
        auto len = std::sqrt(
              this->x * this->x
            + this->y * this->y
            + this->z * this->z);
        (*this) /= len;
    }

    const Vector3Type& getNormal() const
    {
        return this->normal;
    }

    PlaneBase& operator /= (T scalar)
    {
        this->x /= scalar;
        this->y /= scalar;
        this->z /= scalar;
        this->w /= scalar;
        return *this;
    }

public:
    union
    {
        struct { T x, y, z, w; };
        struct
        {
            Vector3Type normal;
            T d;
        };
        T array[4];
    };
};


typedef typename PlaneBase<float> Plane;

}