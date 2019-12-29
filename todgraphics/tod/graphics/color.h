#pragma once
#include <vector>
#include "tod/basetype.h"
#include "tod/stringconv.h"
namespace tod::graphics
{

class ColorF;

//-----------------------------------------------------------------------------
class Color
{
public:
    Color();
    Color(uint8 r, uint8 g, uint8 b);
    Color(uint8 r, uint8 g, uint8 b, uint8 a);
    Color(uint32 color);
    Color(const ColorF& color);
    
    inline String toWebColorStr() const;
    
    inline bool operator == (const Color& rhs) const;
    
public:
    union
    {
        struct
        {
            uint8 r, g, b, a;
        };
        uint32 color;
        uint8 array[4];
    };
};


//-----------------------------------------------------------------------------
String Color::toWebColorStr() const
{
    return String::fromFormat("%02x%02x%02x", this->r, this->g, this->b);
}


//-----------------------------------------------------------------------------
bool Color::operator == (const Color& rhs) const
{
    return (this->r == rhs.r) && (this->g == rhs.g)
        && (this->b == rhs.b) && (this->a == rhs.a);
}


}

namespace tod
{

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Vector3 타입에 대한 String 변환
template <>
class StringConv<const graphics::Color&>
{
public:
    static graphics::Color fromString(const String& value)
    {
        graphics::Color ret;
        std::vector<String> sl;
        value.split(",", sl);
        for (auto i=0u;i<sl.size();++i)
            ret.array[i] = String::atoi(sl[i].c_str());
        return ret;
    }
    static void toString(const graphics::Color& value, String& s)
    {
        s.format("%d,%d,%d", value.r, value.g, value.b, value.a);
    }
};

}

