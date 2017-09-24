#include "tod/graphics/color.h"
#include "tod/graphics/colorf.h"
#include <algorithm>
namespace tod::graphics
{


//-----------------------------------------------------------------------------
Color::Color():
color(0)
{
}


//-----------------------------------------------------------------------------
Color::Color(uint8 r, uint8 g, uint8 b):
r(r), g(g), b(b), a(255)
{
}


//-----------------------------------------------------------------------------
Color::Color(uint8 r, uint8 g, uint8 b, uint8 a):
r(r), g(g), b(b), a(a)
{
}


//-----------------------------------------------------------------------------
Color::Color(uint32 color):color(color)
{
}


//-----------------------------------------------------------------------------
Color::Color(const ColorF& color)
{
    this->r = std::min(static_cast<int>(color.r * 255), 255);
    this->g = std::min(static_cast<int>(color.g * 255), 255);
    this->b = std::min(static_cast<int>(color.b * 255), 255);
    this->a = std::min(static_cast<int>(color.a * 255), 255);
}


}
