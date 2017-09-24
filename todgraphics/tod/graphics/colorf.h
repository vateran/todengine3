#pragma once
namespace tod::graphics
{

class Color;

//-----------------------------------------------------------------------------
class ColorF
{
public:
    ColorF();
    ColorF(float r, float g, float b, float a);
    ColorF(const Color& color);
    
    union
    {
        struct
        {
            float r, g, b, a;
        };
        float array[4];
    };
};

}
