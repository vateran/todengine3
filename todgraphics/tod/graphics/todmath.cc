#include "tod/graphics/todmath.h"
#include <cmath>
#include "tod/basetype.h"
namespace tod::graphics
{
    
//-----------------------------------------------------------------------------
float Math::abs(float value)
{
    return std::abs(value);
}
    

//-----------------------------------------------------------------------------
float Math::sin(float rad)
{
    return std::sin(rad);
}

    
//-----------------------------------------------------------------------------
float Math::asin(float x)
{
    return std::asin(x);
}
    
    
//-----------------------------------------------------------------------------
float Math::cos(float rad)
{
    return std::cos(rad);
}
    
//-----------------------------------------------------------------------------
float Math::acos(float x)
{
    return std::acos(x);
}


//-----------------------------------------------------------------------------
float Math::tan(float rad)
{
    return std::tan(rad);
}


//-----------------------------------------------------------------------------
float Math::atan2(float x, float y)
{
    return std::atan2f(x, y);
}


//-----------------------------------------------------------------------------
float Math::power(float x, float y)
{
    return std::pow(x, y);
}


//-----------------------------------------------------------------------------
float Math::fastInvSqrt(float x)
{
    float half = 0.5f * x;
    int32 i = *reinterpret_cast<int32*>(&x);
    i = 0x5f3759df - (i >> 1);
    x = *reinterpret_cast<float*>(&i);
    return x * (1.5f - half * x * x);
}
    
}
