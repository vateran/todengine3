#include "tod/graphics/todmath.h"
#include <cmath>
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
    
}
