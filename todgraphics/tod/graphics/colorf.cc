#include "tod/graphics/colorf.h"
#include "tod/graphics/color.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
ColorF::ColorF()
    : r(0)
    , g(0)
    , b(0)
    , a(0)
{
}


//-----------------------------------------------------------------------------
ColorF::ColorF(float r, float g, float b, float a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{
}


//-----------------------------------------------------------------------------
ColorF::ColorF(const Color& color)
{
    this->r = color.r / 255.0f;
    this->g = color.g / 255.0f;
    this->b = color.b / 255.0f;
    this->a = color.a / 255.0f;
}

}
