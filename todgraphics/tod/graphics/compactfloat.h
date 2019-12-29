#pragma once
#include <limits>
#include "tod/basetype.h"
namespace tod::graphics
{

template <typename T, uint32 MAX = (std::numeric_limits<T>::max)()>
class CompactFloat
{
public:
    CompactFloat()
    : data(0)
    {
    }

    CompactFloat(float value)
    : data(0)
    {
        this->data = static_cast<T>(std::ceil(value * MAX));
    }

    inline operator float() const
    {
        return static_cast<float>(this->data) / MAX;
    }

    inline T raw() const
    {
        return this->data;
    }

private:
    T data;
};

}